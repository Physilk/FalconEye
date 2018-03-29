#ifndef SCENEOBJECT_HPP
#define SCENEOBJECT_HPP

#include <vector>

#include "gKit/vec.h"

#include "luaObjectInterface.h"
#include "material.h"
#include "ray.h"
#include "resourceManager.h"

using std::vector;
using LuaIntf::_def;

namespace FalconEye {

    class SceneObject {
    protected:
        Material_ptr material;

        SceneObject(const Material_ptr& mat) : material(mat) {}
        SceneObject(const SceneObject &) = default;

    public:

        //NE DOIT PAS ETRE UTILISE, MIT PUBLIC POUR LA COMPATIBILITE LUA
        SceneObject() : material(nullptr) {}
        // /!\

        virtual ~SceneObject() = default;

        virtual bool intersect(const Ray&, Hit&) const { return false; } // = 0;


        virtual Point getCenter() const { return Point(); }//= 0;
        virtual Point getMin() const { return Point(); }//= 0;
        virtual Point getMax() const { return Point(); }//= 0;

        virtual size_t getCount() const { return 1; }

        void setMaterial(const Material_ptr &m) { material = m; }
        Material_ptr getMaterial() { return material; }

        Color getColor(float u, float v) const { return material->getColor(u, v); }
        float getShininess() const { return material->getShininess(); }
        float getReflectivity() const { return material->getReflectivity(); }
        float getRefraction(float u, float v) const { return 1 - material->getColor(u, v).a; }
        float getRefractionValue() const { return material->getRefraction(); }

        LUA_BEGIN_BIND_METHODS(SceneObject)
            LUA_BIND_CONSTRUCTOR_NOARGS
            LUA_END_BIND_METHODS
    };

    class Plan : public SceneObject {
    private:
        Point origin;
        Vector normal;
    public:
        /*
        Plan(Point p, Vector v):	SceneObject(),
        origin(p),
        normal(v)
        {}
        */
        Plan()
            : SceneObject()
            , origin()
            , normal()
        {}

        Plan(Point p, Vector v, const Material_ptr& mat) 
            : SceneObject(mat),
            origin(p),
            normal(v)
        {}

        Plan(const Plan &) = default;

        virtual ~Plan() = default;

        virtual Point getCenter() const override;
        virtual Point getMin() const override;
        virtual Point getMax() const override;

        virtual bool intersect(const Ray&, Hit&) const override;

        void setOrigin(const Point &p) { origin = p; }
        void setNormal(const Vector &v) { normal = v; }

        Point getOrigin() { return origin; }
        Vector getNormal() { return normal; }

        LUA_BEGIN_BIND_METHODS_SUBCLASS_OF(Plan, SceneObject)
            LUA_BIND_CONSTRUCTOR_NOARGS
            LUA_BIND_CONSTRUCTOR_SP(Plan, Point, Vector, Material_ptr&)
            //.addFactory([](Point p, Vector v, Material_ptr m) {return std::shared_ptr<Plan>(new Plan(p, v, m)); })
            LUA_BIND_PROPERTY(Plan, material, getMaterial, setMaterial)
            LUA_BIND_PROPERTY(Plan, origin, getOrigin, setOrigin)
            LUA_BIND_PROPERTY(Plan, normal, getNormal, setNormal)
            LUA_END_BIND_METHODS
    };


    class Sphere : public SceneObject {
    protected:
        Point center;
        float radius;

    public:
        Sphere(Point p, float f, const Material_ptr& mat)
            : SceneObject(mat),
            center(p),
            radius(f)
        {}

        Sphere(const Sphere &) = default;

        virtual ~Sphere() = default;

        virtual Point getCenter() const override;
        virtual Point getMin() const override;
        virtual Point getMax() const override;

        virtual bool intersect(const Ray&, Hit&) const override;

        void setLocalCenter(const Point &p) { center = p; }
        void setRadius(float r) { radius = r; }

        Point getLocalCenter() { return center; }
        float getRadius() { return radius; }

        LUA_BEGIN_BIND_METHODS_SUBCLASS_OF(Sphere, SceneObject)
            LUA_BIND_CONSTRUCTOR_SP(Sphere, Point, float, Material_ptr&)
            //.addFactory([](Point p, float r, Material_ptr m) {return std::shared_ptr<Sphere>(new Sphere(p, r, m)); })
            LUA_BIND_PROPERTY(Sphere, material, getMaterial, setMaterial)
            LUA_BIND_PROPERTY(Sphere, center, getLocalCenter, setLocalCenter)
            LUA_BIND_PROPERTY(Sphere, radius, getRadius, setRadius)
            LUA_END_BIND_METHODS
    };

    /**
    * Ce comporte comme une sphere mais change la direction de la normale revoyee par intersect
    * */
    class InverseSphere : public Sphere {
    public:
        InverseSphere(Point p, float f, const Material_ptr& mat)
            : Sphere(p, f, mat)
        {}
        InverseSphere(const InverseSphere &) = default;

        virtual ~InverseSphere() = default;

        virtual bool intersect(const Ray&, Hit&) const override;

        LUA_BEGIN_BIND_METHODS_SUBCLASS_OF(InverseSphere, Sphere)
            LUA_BIND_CONSTRUCTOR_SP(InverseSphere, Point, float, Material_ptr&)
            //.addFactory([](Point p, float r, Material_ptr m) {return std::shared_ptr<InverseSphere>(new InverseSphere(p, r, m)); })
            LUA_BIND_PROPERTY(InverseSphere, material, getMaterial, setMaterial)
            LUA_BIND_PROPERTY(InverseSphere, center, getLocalCenter, setLocalCenter)
            LUA_BIND_PROPERTY(InverseSphere, radius, getRadius, setRadius)
            LUA_END_BIND_METHODS
    };

    class Triangle : public SceneObject {

    protected:
        Point v0;
        Point v1;
        Point v2;

        vec2 v0_uv;
        vec2 v1_uv;
        vec2 v2_uv;

        Triangle(const Material_ptr &mat) : SceneObject(mat) {}

    public:

        Triangle(Point p0, Point p1, Point p2, const Material_ptr& mat,
            vec2 p0_uv = vec2(0, 0), vec2 p1_uv = vec2(0, 0),
            vec2 p2_uv = vec2(0, 0))
            : SceneObject(mat),
            v0(p0),
            v1(p1),
            v2(p2)
        {}

        Triangle(const Triangle&) = default;

        virtual ~Triangle() = default;

        virtual Point getCenter() const override;
        virtual Point getMin() const override;
        virtual Point getMax() const override;

        virtual bool intersect(const Ray&, Hit&) const override;

        Point getV0() { return v0; }
        Point getV1() { return v1; }
        Point getV2() { return v2; }

        vec2 getV0_uv() { return v0_uv; }
        vec2 getV1_uv() { return v1_uv; }
        vec2 getV2_uv() { return v2_uv; }

        void setV0(const Point &p) { v0 = p; }
        void setV1(const Point &p) { v1 = p; }
        void setV2(const Point &p) { v2 = p; }

        void setV0_uv(const vec2 &v) { v0_uv = v; }
        void setV1_uv(const vec2 &v) { v1_uv = v; }
        void setV2_uv(const vec2 &v) { v2_uv = v; }


        LUA_BEGIN_BIND_METHODS_SUBCLASS_OF(Triangle, Sphere)
            LUA_BIND_CONSTRUCTOR_SP(Triangle, Point, Point, Point, Material_ptr&, _opt<vec2>, _opt<vec2>, _opt<vec2>)
            //.addFactory([](Point p0, Point p1, Point p2, Material_ptr m) {return std::shared_ptr<Triangle>(new Triangle(p0, p1, p2, m)); })
            LUA_BIND_PROPERTY(Triangle, v0, getV0, setV0)
            LUA_BIND_PROPERTY(Triangle, v1, getV1, setV1)
            LUA_BIND_PROPERTY(Triangle, v2, getV2, setV2)
            LUA_BIND_PROPERTY(Triangle, v0_uv, getV0_uv, setV0_uv)
            LUA_BIND_PROPERTY(Triangle, v1_uv, getV1_uv, setV1_uv)
            LUA_BIND_PROPERTY(Triangle, v2_uv, getV2_uv, setV2_uv)
            LUA_END_BIND_METHODS
    };

    using SceneObject_ptr = std::shared_ptr<SceneObject>;

    class MeshObject;

    class MeshTriangle : public SceneObject {
    private:
        const MeshObject * parent;
        //index qui donne, après avoir été multiplié par 3, la position voulue dans parent->mesh->indices
        unsigned int index;

        Point barycentre;
        Point min;
        Point max;

        Vector normal;

    public:
        MeshTriangle() = delete;
        MeshTriangle(const MeshObject *p, unsigned int i, const Material_ptr &mat);
        MeshTriangle(const MeshTriangle&) = default;


        virtual bool intersect(const Ray&, Hit&) const override;

        virtual Point getCenter() const override;
        virtual Point getMin() const override;
        virtual Point getMax() const override;
    };

    //forward declaration
    class BBoxBinTree;

    class MeshObject : public SceneObject {
    private:
        Mesh_ptr mesh;
        Transform model;

        vector<SceneObject_ptr> triangles;

        Point center;
        Point min;
        Point max;

        BBoxBinTree * bboxBinTree;

        void initTriangles(const Transform&);

    public:
        MeshObject() = delete;
        MeshObject(const Transform &t, const char*, const Material_ptr& mat);
        MeshObject(const MeshObject &) = default;

        ~MeshObject();

        virtual Point getCenter() const override;
        virtual Point getMin() const override;
        virtual Point getMax() const override;

        virtual size_t getCount() const override { return triangles.size(); }

        virtual bool intersect(const Ray&, Hit&) const override;

        const Mesh_ptr& getMesh() const { return mesh; }
        Mesh_ptr& getMesh() { return mesh; }

        friend class MeshTriangle;

    public:
        struct TrianglePointPositions
        {
            Point v0;
            Point v1;
            Point v2;

            TrianglePointPositions(const Point &p0, const Point &p1, const Point &p2)
                : v0(p0)
                , v1(p1)
                , v2(p2)
            {}
        };

        struct TrianglePointNormals
        {
            Vector n0;
            Vector n1;
            Vector n2;

            TrianglePointNormals(const Vector &p0, const Vector &p1, const Vector &p2)
                : n0(p0)
                , n1(p1)
                , n2(p2)
            {}
        };

        struct TrianglePointTexcoord
        {
            vec2 t0;
            vec2 t1;
            vec2 t2;

            TrianglePointTexcoord(const vec2 &p0, const vec2 &p1, const vec2 &p2)
                : t0(p0)
                , t1(p1)
                , t2(p2)
            {}
        };

        struct TrianglePointColor
        {
            vec4 t0;
            vec4 t1;
            vec4 t2;

            TrianglePointColor(const vec4 &p0, const vec4 &p1, const vec4 &p2)
                : t0(p0)
                , t1(p1)
                , t2(p2)
            {}
        };

    private:
        bool useColors;
        bool useUV;
        bool smoothTriangles;

        vector<TrianglePointPositions> positions;
        vector<TrianglePointNormals> normals;

        void fillVectorsWithTransformedData(const Transform &t);

        const TrianglePointPositions &getPositions(size_t triangle_index) const { return positions[triangle_index]; }
        const TrianglePointTexcoord getTexcoords(size_t triangle_index) const;
        const TrianglePointNormals &getNormals(size_t triangle_index) const { return normals[triangle_index]; }
        const TrianglePointColor getColors(size_t triangle_index) const;

        bool hasNormals() const { return normals.size() > 0; }
        bool hasTexcoord() const { return mesh->texcoords().size() > 0; }
        bool hasColors() const { return mesh->colors().size() > 0; }

    public:
        void setUseColors(bool b) { useColors = b; }
        void setUseUV(bool b) { useUV = b; }
        void setSmoothTriangles(bool b) { smoothTriangles = b; }

        bool getUseColors() const { return useColors; }
        bool getUseUV() const { return useUV; }
        bool getSmoothTriangles() const { return smoothTriangles; }

        LUA_BEGIN_BIND_METHODS_SUBCLASS_OF(MeshObject, SceneObject)
            LUA_BIND_CONSTRUCTOR_SP(MeshObject, Transform, const char*, Material_ptr)
            //.addFactory([](Transform t, const char* s, Material_ptr m) {return std::shared_ptr<MeshObject>(new MeshObject(t, s, m)); })
            LUA_BIND_PROPERTY(MeshObject, useColors, getUseColors, setUseColors)
            LUA_BIND_PROPERTY(MeshObject, useUV, getUseUV, setUseUV)
            LUA_BIND_PROPERTY(MeshObject, smoothTriangles, getSmoothTriangles, setSmoothTriangles)
            LUA_END_BIND_METHODS

    };
} // end namespace FalconEye



#endif
