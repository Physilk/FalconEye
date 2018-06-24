#ifndef BINTREE_HPP
#define BINTREE_HPP

#include <vector>

#include "bbox.h"
#include "SceneObject.h"

namespace FalconEye {
    class SceneObject;
    class BBoxBinTree;

    class BBoxBinTreeNode {

    private:
        BBoxBinTreeNode * d;
        BBoxBinTreeNode * g;

        BBox bbox;
        std::vector<SceneObject *> objects;

    public:
        BBoxBinTreeNode(BBoxBinTreeNode&& other);
        BBoxBinTreeNode() = default;
        BBoxBinTreeNode(BBox b, const std::vector<SceneObject *> &o) : d(nullptr),
            g(nullptr),
            bbox(b),
            objects(o)
        {}
        ~BBoxBinTreeNode();

        std::vector<SceneObject *> getObjects() const { return objects; }
        BBox getBBox() const { return bbox; }

        bool intersect(const Ray &, Hit &) const;

        void split(size_t max_objects_per_node);

        friend class BBoxBinTree;
    };

    class BBoxBinTree {
    private:
        BBoxBinTreeNode parent;

        BBoxBinTree() = delete;

        std::vector<SceneObject *> infinite_objects;

    public:
        BBoxBinTree(BBox b, const std::vector<SceneObject *> &o, size_t s);

        ~BBoxBinTree() = default;

        bool intersect(const Ray &, Hit &) const;

        void split(size_t max_objects_per_node = 10) { parent.split(max_objects_per_node); }
    };
} // end namespace FalconEye


#endif
