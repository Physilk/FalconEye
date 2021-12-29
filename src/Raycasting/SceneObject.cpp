#include <limits>

#include "bintree.h"
#include "SceneObject.h"
#include "Utils/tools.h"

#include <cmath>

using std::pow;
using std::atan2;
using std::asin;

namespace FalconEye {

    // -----------------------------------------------------

    bool Plan::intersect(const Ray& ray, Hit& hit) const {
        float div = dot(normal, ray.direction);
        if (std::abs(div) > 0.00001f)
        {
            Vector temp = origin - ray.origin;
            float div2 = dot(temp, normal);

            float res = div2 / div;

            if (res >= 0.00001f)
            {
                hit.p = ray.origin + res * ray.direction;
                hit.n = normal;
                hit.t = res;

                hit.p_object = this;
                return true;
            }
        }

        return false;
    }

    // -----------------------------------------------------

    Point Plan::getCenter() const {
        return origin;
    }

    // -----------------------------------------------------

    Point Plan::getMin() const {
        return Point(-std::numeric_limits<float>::max(),
            -std::numeric_limits<float>::max(),
            -std::numeric_limits<float>::max()
        );
    }

    // -----------------------------------------------------

    Point Plan::getMax() const {
        return Point(std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max()
        );
    }

    // -----------------------------------------------------
    // -----------------------------------------------------

    bool Sphere::intersect(const Ray&ray, Hit &hit) const {

        float t0, t1;
        Vector L = ray.origin - center;
        float a = dot(ray.direction, ray.direction);
        float b = 2 * dot(ray.direction, L);
        float c = dot(L, L) - pow(radius, 2);
        if (!Tools::solveQuadratic(a, b, c, t0, t1)) return false;

        if (t0 > t1) std::swap(t0, t1);

        if (t0 < 0)
        {
            t0 = t1; // if t0 is negative, let's use t1 instead
            if (t0 < 0)
            {
                return false; // both t0 and t1 are negative
            }
        }

        hit.t = t0;
        hit.p = ray.origin + t0 * ray.direction;
        hit.n = normalize(Vector(center, hit.p));

        hit.p_object = this;

        Vector vp = normalize(hit.p - center);
        hit.uv.x = 0.5f - (atan2f(vp.z, vp.x)) / (2 * M_PI);
        hit.uv.y = 0.5f + (asinf(vp.y)) / M_PI;
        return true;
    }

    // -----------------------------------------------------

    Point Sphere::getCenter() const {
        //return model(center);
        return center;
    }

    // -----------------------------------------------------

    Point Sphere::getMin() const {
        Point ret = getCenter();
        ret.x -= radius;
        ret.y -= radius;
        ret.z -= radius;
        return ret;
    }

    // -----------------------------------------------------

    Point Sphere::getMax() const {
        Point ret = getCenter();
        ret.x += radius;
        ret.y += radius;
        ret.z += radius;
        return ret;
    }

    // -----------------------------------------------------
    // -----------------------------------------------------

    bool InverseSphere::intersect(const Ray &ray, Hit &hit) const {
        bool b = Sphere::intersect(ray, hit);
        if (b)
            hit.n = Vector(0, 0, 0) - hit.n;
        return b;
    }

    // -----------------------------------------------------
    // -----------------------------------------------------

    bool Triangle::intersect(const Ray& ray, Hit& hit) const {
        Vector e1 = v1 - v0;
        Vector e2 = v2 - v0;
        Vector T = ray.origin - v0;

        Vector P = cross(ray.direction, e2);
        Vector Q = cross(T, e1);

        Vector result = (1 / dot(P, e1)) * Vector(dot(Q, e2), dot(P, T), dot(Q, ray.direction));
        if (result.x <= 0 || result.y < 0 || result.z < 0 || (result.y + result.z) > 1)
            return false;

        hit.t = result.x;
        hit.p = ray.origin + hit.t * ray.direction;
        hit.n = normalize(cross(e1, e2));
        hit.p_object = this;

        return true;
    }

    // -----------------------------------------------------

    Point Triangle::getCenter() const {
        return Point((v0.x + v1.x + v2.x) / 3, (v0.y + v1.y + v2.y) / 3, (v0.z + v1.z + v2.z) / 3);
    }

    // -----------------------------------------------------

    Point Triangle::getMin() const {
        Point ret = v0;
        if (ret.x > v1.x)
            ret.x = v1.x;
        if (ret.x > v2.x)
            ret.x = v2.x;
        if (ret.y > v1.y)
            ret.y = v1.y;
        if (ret.y > v2.y)
            ret.y = v2.y;
        if (ret.z > v1.z)
            ret.z = v1.z;
        if (ret.z > v2.z)
            ret.z = v2.z;

        return ret;
    }

    // -----------------------------------------------------

    Point Triangle::getMax() const {
        Point ret = v0;
        if (ret.x < v1.x)
            ret.x = v1.x;
        if (ret.x < v2.x)
            ret.x = v2.x;
        if (ret.y < v1.y)
            ret.y = v1.y;
        if (ret.y < v2.y)
            ret.y = v2.y;
        if (ret.z < v1.z)
            ret.z = v1.z;
        if (ret.z < v2.z)
            ret.z = v2.z;

        return ret;
    }

    // -----------------------------------------------------
    // -----------------------------------------------------

    MeshTriangle::MeshTriangle(const MeshObject *p, unsigned int i, const Material_ptr &mat) : SceneObject(mat),
        parent(p),
        index(i)
    {
        const MeshObject::TrianglePointPositions &pos = parent->getPositions(index);
        barycentre = Point((pos.v0.x + pos.v1.x + pos.v2.x) / 3, (pos.v0.y + pos.v1.y + pos.v2.y) / 3, (pos.v0.z + pos.v1.z + pos.v2.z) / 3);

        if (parent->normals.size() > 0) {
            const MeshObject::TrianglePointNormals &normals = parent->getNormals(index);
            const Vector &n0 = normals.n0;
            const Vector &n1 = normals.n1;
            const Vector &n2 = normals.n2;

            normal = normalize(n0 + n1 + n2);
        }
        else {
            Vector e1 = pos.v1 - pos.v0;
            Vector e2 = pos.v2 - pos.v0;
            normal = normalize(cross(e1, e2));
        }

        //min
        min = pos.v0;
        if (min.x > pos.v1.x)
            min.x = pos.v1.x;
        if (min.x > pos.v2.x)
            min.x = pos.v2.x;
        if (min.y > pos.v1.y)
            min.y = pos.v1.y;
        if (min.y > pos.v2.y)
            min.y = pos.v2.y;
        if (min.z > pos.v1.z)
            min.z = pos.v1.z;
        if (min.z > pos.v2.z)
            min.z = pos.v2.z;

        //max
        max = pos.v0;
        if (max.x < pos.v1.x)
            max.x = pos.v1.x;
        if (max.x < pos.v2.x)
            max.x = pos.v2.x;
        if (max.y < pos.v1.y)
            max.y = pos.v1.y;
        if (max.y < pos.v2.y)
            max.y = pos.v2.y;
        if (max.z < pos.v1.z)
            max.z = pos.v1.z;
        if (max.z < pos.v2.z)
            max.z = pos.v2.z;
    }

    // -----------------------------------------------------

    bool MeshTriangle::intersect(const Ray &ray, Hit &hit) const {
        static const float ESPSILON = 0.0000001f;

        const MeshObject::TrianglePointPositions &pos = parent->getPositions(index);
        const Point &v0 = pos.v0;
        const Point &v1 = pos.v1;
        const Point &v2 = pos.v2;

        Vector e1 = v1 - v0;
        Vector e2 = v2 - v0;

        Vector h = cross(ray.direction, e2);
        float den = dot(e1, h);
        if (den > -ESPSILON && den < ESPSILON)
            return false;
        float f = 1 / den;
        Vector s = ray.origin - v0;
        float u = f * dot(s, h);
        if (u < 0.0f || u > 1.0f)
            return false;
        Vector q = cross(s, e1);
        float v = f * dot(ray.direction, q);
        if (v < 0.0f || u + v > 1.0f)
            return false;
        hit.t = f * dot(e2, q);
        if (hit.t < ESPSILON)
            return false;

        hit.p = ray.origin + hit.t * ray.direction;

        const float &&denominateur = (v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y);
        const float &&a = (hit.p.x - v2.x);
        const float &&b = (hit.p.y - v2.y);
        const float &&coefA = 1 - ((v1.y - v2.y) * a + (v2.x - v1.x) * b) / denominateur;
        const float &&coefB = 1 - ((v2.y - v0.y) * a + (v0.x - v2.x) * b) / denominateur;
        const float &&coefC = 1 - coefA - coefB;

        if (parent->getSmoothTriangles() && parent->normals.size() > 0) {
            const MeshObject::TrianglePointNormals &normals = parent->getNormals(index);
            const Vector &n0 = normals.n0;
            const Vector &n1 = normals.n1;
            const Vector &n2 = normals.n2;

            hit.n = normalize((coefA * n0) + (coefB * n1) + (coefC * n2)); //suposed to be already normalized but not sure cuz of float precision
        }
        else {
            hit.n = normal;
        }

        hit.uv.x = 0.0f;
        hit.uv.y = 0.0f;

        if (parent->getUseUV() && parent->hasTexcoord()) {
            const MeshObject::TrianglePointTexcoord &texcoords = parent->getTexcoords(index);
            const vec2 &v0_uv = texcoords.t0;
            const vec2 &v1_uv = texcoords.t1;
            const vec2 &v2_uv = texcoords.t2;

            hit.uv.x = v0_uv.x * coefA + v1_uv.x * coefB + v2_uv.x * coefC;
            hit.uv.y = v0_uv.y * coefA + v1_uv.y * coefB + v2_uv.y * coefC;
        }
        else if (parent->getUseColors() && parent->hasColors()) {
            const MeshObject::TrianglePointColor &colors = parent->getColors(index);
            const vec4 &c0 = colors.t0;
            const vec4 &c1 = colors.t1;
            const vec4 &c2 = colors.t2;

            Vector rgb = Vector(c0.x, c0.y, c0.z) * coefA + Vector(c1.x, c1.y, c1.z) * coefB + Vector(c2.x, c2.y, c2.z) * coefC;
            float a = c0.z * coefA + c1.z * coefB + c2.z * coefC;

            hit.c = Color(rgb.x, rgb.y, rgb.z, a);
        }

        hit.p_object = this;
        return true;
    }

    // -----------------------------------------------------

    Point MeshTriangle::getCenter() const {
        return barycentre;
    }

    // -----------------------------------------------------

    Point MeshTriangle::getMin() const {
        return min;
    }

    // -----------------------------------------------------

    Point MeshTriangle::getMax() const {
        return max;
    }

    // -----------------------------------------------------
    // -----------------------------------------------------

    MeshObject::MeshObject(const Transform &t, const char*s, const Material_ptr& mat)
        : SceneObject(mat)
        , mesh(ResourceManager::Instance().requestMesh(s))
        , useColors(false)
        , useUV(false)
        , smoothTriangles(false)
    {
        fillVectorsWithTransformedData(t);
        initTriangles(t);
        bboxBinTree = new BBoxBinTree(BBox(min, max), Tools::sharedPtrVecToPtrVec(triangles), 10);
    }

    // -----------------------------------------------------

    MeshObject::~MeshObject() {
        delete bboxBinTree;
    }

    // -----------------------------------------------------

    void MeshObject::initTriangles(const Transform &t) {
        model = t;
        center = model(Point(0, 0, 0));
        const size_t size = positions.size();
        triangles.reserve(size);
        if (size < 1)
            return;

        MeshTriangle * triangle = new MeshTriangle(this, 0, material);
        triangles.push_back(SceneObject_ptr(triangle));
        min = triangle->getMin();
        max = triangle->getMax();
        for (size_t i = 1; i < size; ++i) {
            triangle = new MeshTriangle(this, i, material);
            triangles.push_back(SceneObject_ptr(triangle));

            Point t_min = triangle->getMin();
            Point t_max = triangle->getMax();

            if (min.x > t_min.x)
                min.x = t_min.x;
            if (min.y > t_min.y)
                min.y = t_min.y;
            if (min.z > t_min.z)
                min.z = t_min.z;

            if (max.x < t_max.x)
                max.x = t_max.x;
            if (max.y < t_max.y)
                max.y = t_max.y;
            if (max.z < t_max.z)
                max.z = t_max.z;
        }
    }

    // -----------------------------------------------------

    bool MeshObject::intersect(const Ray &ray, Hit &hit) const {

        return bboxBinTree->intersect(ray, hit);
    }

    // -----------------------------------------------------

    Point MeshObject::getCenter() const {
        return center;
    }

    // -----------------------------------------------------

    Point MeshObject::getMin() const { return min; }
    Point MeshObject::getMax() const { return max; }

    // -----------------------------------------------------

    void MeshObject::fillVectorsWithTransformedData(const Transform & t)
    {
        auto& original_positions = mesh->positions();
        auto& original_normals = mesh->normals();
        auto& original_texcoord = mesh->texcoords();

        size_t until = original_positions.size() / 3;
        positions.reserve(until);
        for (size_t i = 0; i < until; ++i)
        {
            const Point &&p0 = t(Point(original_positions[i * 3 + 0]));
            const Point &&p1 = t(Point(original_positions[i * 3 + 1]));
            const Point &&p2 = t(Point(original_positions[i * 3 + 2]));

            positions.push_back(TrianglePointPositions(
                p0,
                p1,
                p2
            ));
        }
        until = original_normals.size() / 3;
        normals.reserve(until);
        for (size_t i = 0; i < until; ++i)
        {
            const Vector &&p0 = t.normal()(Vector(original_normals[i * 3 + 0]));
            const Vector &&p1 = t.normal()(Vector(original_normals[i * 3 + 1]));
            const Vector &&p2 = t.normal()(Vector(original_normals[i * 3 + 2]));

            normals.push_back(TrianglePointNormals(
                p0,
                p1,
                p2
            ));
        }
    }

    // -----------------------------------------------------

    const MeshObject::TrianglePointTexcoord MeshObject::getTexcoords(size_t triangle_index) const
    {
        auto texcoords = mesh->texcoords();
        return MeshObject::TrianglePointTexcoord(texcoords[triangle_index * 3 + 0],
            texcoords[triangle_index * 3 + 1],
            texcoords[triangle_index * 3 + 2]
        );
    }

    // -----------------------------------------------------

    const MeshObject::TrianglePointColor MeshObject::getColors(size_t triangle_index) const
    {
        auto colors = mesh->colors();
        return MeshObject::TrianglePointColor(colors[triangle_index * 3 + 0],
            colors[triangle_index * 3 + 1],
            colors[triangle_index * 3 + 2]
        );
    }

} // end namespace FalconEye
