#include <chrono>

#include "gKit/image.h"
#include "gKit/image_io.h"

#include "bintree.h"
#include "scene.h"
#include "Utils/tools.h"

#include "ray.h"

#include <cmath>

namespace FalconEye {

    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------

    /*void Scene::renderScene(const char *filename, SceneRenderOption_ptr ro) {

        if (bboxBinTree == nullptr)
            preProcess();
            
        Image output = SceneRenderer::renderScene(*this, ro);
        write_image(output, filename);
        return;
    }*/

    // -----------------------------------------------------------------------

    bool Scene::intersect(const Ray&ray, Hit &hit, const SceneObject* ignore) {
        if (bboxBinTree == nullptr) {
            bool current_hit = false;
            bool ever_hit = false;
            Hit closest_hit;

            for (unsigned int i = 0; i < objects.size(); ++i) {
                if (objects[i].get() == ignore)
                    continue;
                if (objects[i]->canIntersect(ray) && (current_hit = objects[i]->intersect(ray, hit))) {
                    if (!ever_hit || hit.t < closest_hit.t) {
                        closest_hit = hit;
                    }
                    ever_hit = true;
                }
            }

            hit = closest_hit;
            return ever_hit;
        }
        return bboxBinTree->intersect(ray, hit);
    }

    // -----------------------------------------------------------------------

    bool Scene::intersect(const Ray& ray, const SceneObject* ignore) {
        Hit tmp_hit;
        if (bboxBinTree == nullptr) {
            for (unsigned int i = 0; i < objects.size(); ++i) {
                if (objects[i].get() == ignore)
                    continue;
                if (objects[i]->canIntersect(ray) && objects[i]->intersect(ray, tmp_hit)) {
                    return true;
                }
            }

            return false;
        }
        return bboxBinTree->intersect(ray, tmp_hit);
    }

    // -----------------------------------------------------------------------

    Color Scene::processPixelColor(const Ray & ray, const Hit &hit, const size_t possible_reflection_bounce) {
        const Material_ptr object_material = hit.p_object->getMaterial();
        const SampleParameters sampleParam = SampleParameters(hit.uv);
        Color albedoColor = object_material->getColor(sampleParam);
        Color reflectColor = Color(0, 0, 0);
        Color refractColor = Color(0, 0, 0);
        float reflectivity = object_material->getReflectivity(sampleParam);
        float transparency = 1 - albedoColor.a; // 1 - opacity
        const float delta = 0.001f;
        const float fresnel_factor_0 = 0.8f;

        if (reflectivity > 1)
            reflectivity = 1;
        //partie sur la reflection
        if (reflectivity > 0 && possible_reflection_bounce > 0) {
            //lancer un rayon du point de l'object en direction du vecteur reflet
            /*Vector reversOrigToHitVector = normalize(Vector(hit.p, ray.origin));
            Vector reflectionVector = 2 * dot(reversOrigToHitVector, hit.n) * hit.n - reversOrigToHitVector;*/

            Vector reflectionVector = Tools::reflect(hit.n, ray.direction);

            Ray newRay;
            Hit delta_hit = hit;
            delta_hit.p = hit.p + delta * reflectionVector;
            /*newRay.origin = delta_hit.p;
            newRay.direction = reflectionVector;*/
            newRay = make_ray(delta_hit.p, reflectionVector, ray.n);

            Hit newHit;

            //lancer du rayon
            if (intersect(newRay, newHit, hit.p_object)) {
                /*Color newColor = processPixelColor(newRay, newHit, possible_reflection_bounce - 1);
                albedoColor = (1 - reflectivity) * albedoColor + reflectivity * newColor;*/
                reflectColor = processPixelColor(newRay, newHit, possible_reflection_bounce - 1);
            }

        }
        if (transparency > 0 && possible_reflection_bounce > 0) {

            const float n2 = object_material->getRefraction(sampleParam);

            Vector refractionVector = Tools::refract(hit.n, ray.direction, ray.n, n2);


            Ray newRay;
            Hit delta_hit = hit;
            delta_hit.p = hit.p + delta * refractionVector;
            newRay = make_ray(delta_hit.p, refractionVector, n2);

            Hit newHit;

            if (refractionVector.x == 0 && refractionVector.y == 0 && refractionVector.y == 0) {
                refractColor = albedoColor;
            }
            else if (intersect(newRay, newHit, hit.p_object)) {
                /*Color newColor = processPixelColor(newRay, newHit, possible_reflection_bounce - 1);
                albedoColor = (1 - reflectivity) * albedoColor + reflectivity * newColor;*/
                refractColor = processPixelColor(newRay, newHit, possible_reflection_bounce - 1);
            }

        }

        float fresnel_factor = Tools::fresnelFactor(fresnel_factor_0, hit.n, ray.direction);
        albedoColor = (1 - reflectivity - transparency) * albedoColor + reflectivity * reflectColor + transparency * refractColor;

        float shininess = object_material->getShininess(sampleParam);
        Color ambientColor = albedoColor * shadow_coeficient;


        Color return_color = Color(0.0f, 0.0f, 0.0f, 1.0f);//ambientColor;



        for (auto it = lights.begin(); it != lights.end(); ++it) {
            Color specColor = (*it)->getColor();
            //Color light_color = (*it)->getColor();
            Point light_position = (*it)->getPosition();
            float light_range = (*it)->getRange();

            //std::cout << "light range=" << light_range << "\n";

            Hit delta_hit = hit;
            delta_hit.p = hit.p + delta * normalize(light_position - hit.p);
            Ray ray_to_light = make_ray(delta_hit.p, light_position);
            float dist = length(light_position - hit.p);

            //std::cout << "dist=" << dist << "\n";

            if (dist >= light_range)
                continue;

            Hit tmp_hit;
            bool b = intersect(ray_to_light, tmp_hit/*, hit.p_object*/);
            //if(b)std::cout << "tmp_hit.t=" << tmp_hit.t << "\n";
            if (b && tmp_hit.t < dist) {
                continue; //ICI POUR GERE LES OBJETS TRANSPARENT
            }

            Color light_color = (*it)->attenuation(delta_hit.p);

            float cosTheta = dot(hit.n, normalize(Vector(hit.p, light_position)));
            if (cosTheta < 0)
                cosTheta = 0;
            else if (cosTheta > 1)
                cosTheta = 1;

            if (reflectivity > cosTheta)
                cosTheta = reflectivity;
            //std::cout << "cosTheta=" << cosTheta << "\n";
            float specular = 0.0f;

            if (cosTheta > 0) {
                Vector viewDir = Vector(0, 0, 0) - ray.direction;

                if (shininess > 1) {
                    Vector halfDir = normalize(ray_to_light.direction + viewDir);
                    float specularAngle = dot(halfDir, hit.n);
                    if (specularAngle < 0)
                        specularAngle = 0;
                    else if (specularAngle > 1)
                        specularAngle = 1;
                    specular = std::pow(specularAngle, shininess);
                }
            }

            return_color = return_color + light_color * (albedoColor * cosTheta + specular * specColor);
        }

        return_color.a = 1.0f;
        return return_color;

    }

    // -----------------------------------------------------------------------

    void Scene::addObject(SceneObject_ptr o) {
        objects.push_back(o);

        //update des limites
        Point min = o->getMin();
        Point max = o->getMax();

        if (boundMax.x < max.x)
            boundMax.x = max.x;
        if (boundMax.y < max.y)
            boundMax.y = max.y;
        if (boundMax.z < max.z)
            boundMax.z = max.z;

        if (boundMin.x > min.x)
            boundMin.x = min.x;
        if (boundMin.y > min.y)
            boundMin.y = min.y;
        if (boundMin.z > min.z)
            boundMin.z = min.z;
    }

    // -----------------------------------------------------------------------

    void Scene::preProcess() {
        if (bboxBinTree != nullptr)
            delete bboxBinTree;
        bboxBinTree = new BBoxBinTree(BBox(boundMin, boundMax), Tools::sharedPtrVecToPtrVec(objects), 11);
    }

	void Scene::LuaRenderScene(SceneRenderOption_ptr options)
	{
		SceneRenderer_ptr sceneRenderer = std::make_shared<SceneRenderer>();
        RenderingContext_ptr renderingContext = std::make_shared<RenderingContext>(this, sceneRenderer.get());

		sceneRenderer->SetContext(renderingContext);

		Image image = Image(options->getWidth(), options->getHeight());
		auto startTime = std::chrono::high_resolution_clock::now();

        std::vector<Threading::TJob_ptr> renderingJobs;
        sceneRenderer->renderScene(&image, renderingJobs, options);

        for (auto Job : renderingJobs)
        {
            Job->WaitFinish();
        }

		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count();
		std::cout << "render time: " << ms << "ms" << std::endl;
	}

	// -----------------------------------------------------------------------

    void Scene::initBounds() {
        float limit = std::numeric_limits<float>::max();
        boundMin = Point(limit, limit, limit);
        boundMax = Point(-limit, -limit, -limit);
    }

    // -----------------------------------------------------------------------

    Scene::~Scene() {
        delete bboxBinTree;
    }

    // -----------------------------------------------------------------------

    size_t Scene::getCount() {
        size_t count = 0;
        for (size_t i = 0; i < objects.size(); ++i) {
            count += objects[i]->getCount();
        }
        return count;
    }

} // end namespace FalconEye

