// File:        impl_red_black_tree.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-10-20 by leoxiang

#pragma once

#include <set>
#include <utility>
#include <cstddef>
#include <iterator>

namespace lsf {
namespace container {
namespace detail {

////////////////////////////////////////////////////////////
// RBTreeNode
////////////////////////////////////////////////////////////
template <typename ElemType, typename SizeType>
struct RBTreeNode {
    SizeType lchild_pos;
    SizeType rchild_pos;
    SizeType parent_pos;
    ElemType data;
    uint8_t color;
};

////////////////////////////////////////////////////////////
// RBTreeState
////////////////////////////////////////////////////////////
template <typename ElemType, typename SizeType>
class RBTreeState {
public:
    typedef ElemType value_type;
    typedef SizeType size_type;
    typedef RBTreeNode<ElemType, SizeType> node_type;
    typedef RBTreeState<ElemType, SizeType> this_type;

    static const size_type NPOS = 0;

    static const uint8_t BLACK = 0x0;
    static const uint8_t RED = 0x1;
    static const uint8_t EMPTY = 0x2;

public:
    void Init(size_type byte_size) {
        _root_pos = NPOS;
        _empty_head_pos = 1;
        _max_size = (byte_size - sizeof(this_type)) / sizeof(node_type);
        _size = 0;
        _elem_byte_size = sizeof(value_type);

        // make empty list a loop
        // use lchild and rchild as prev_pos and next_pos
        for (size_type pos = 1; pos < _max_size; pos++) {
            SetLChildPos(pos, pos - 1);
            SetRChildPos(pos, pos + 1);
            SetColor(pos, EMPTY);
        }
        SetLChildPos(1, _max_size - 1);
        SetRChildPos(_max_size - 1, 1);

        // NIL node must be black
        SetColor(NPOS, BLACK);
        SetParentPos(NPOS, NPOS);
        SetLChildPos(NPOS, NPOS);
        SetRChildPos(NPOS, NPOS);
    }

    //////////////////////////////////////////////////
    // for debug
    //////////////////////////////////////////////////
    // red black tree feature
    // 1. all node is red or black
    // 2. root node is black
    // 3. all NPOS node is black
    // 4. red node can only have black child
    // 5. any path to a leaf contains same number of black node
    bool CheckConsist() const {
        if (GetColor(_empty_head_pos) != EMPTY && _empty_head_pos != NPOS) {
            return false;
        }
        if (GetColor(_root_pos) != BLACK) {
            return false;
        }

        if (GetColor(NPOS) != BLACK) {
            return false;
        }

        // check empty list
        std::set<size_t> empty_sets;
        if (_empty_head_pos != NPOS) empty_sets.insert(_empty_head_pos);
        for (size_t pos = GetRChildPos(_empty_head_pos); pos != _empty_head_pos && _empty_head_pos != NPOS;
             pos = GetRChildPos(pos)) {
            if (GetColor(pos) != EMPTY) {
                return false;
            }
            if (!(empty_sets.insert(pos).second)) {
                return false;
            }
        }
        if (empty_sets.size() != max_size() - size()) {
            return false;
        }

        // check use list
        std::set<size_t> use_sets;
        for (size_t pos = FindSmallestPos(); pos != NPOS; pos = FindNextBigPos(pos)) {
            if (GetColor(pos) != RED && GetColor(pos) != BLACK) {
                return false;
            }
            if (!(use_sets.insert(pos).second)) {
                return false;
            }
        }
        if (use_sets.size() != size()) {
            std::cout << use_sets.size() << "," << size() << std::endl;
            return false;
        }

        return true;
    }

    //////////////////////////////////////////////////
    // FindNode
    //////////////////////////////////////////////////
    std::pair<size_type, size_type> FindNode(value_type const &val) {
        size_type pos = _root_pos;
        size_type parent_pos = NPOS;

        while (pos != NPOS) {
            // already exist
            if (*GetDataPtr(pos) == val) break;

            if (*GetDataPtr(pos) < val) {
                parent_pos = pos;
                pos = GetRChildPos(pos);
            } else {
                parent_pos = pos;
                pos = GetLChildPos(pos);
            }
        }
        return std::make_pair(pos, parent_pos);
    }

    size_type FindSmallestPos() const { return FindSmallestPos(_root_pos); }

    size_type FindSmallestPos(size_type from_pos) const {
        if (from_pos == NPOS) return NPOS;

        size_type pos;
        for (pos = from_pos; GetLChildPos(pos) != NPOS; pos = GetLChildPos(pos))
            ;
        return pos;
    }

    size_type FindBiggestPos() const { return FindBiggestPos(_root_pos); }

    size_type FindBiggestPos(size_type from_pos) const {
        if (from_pos == NPOS) return NPOS;

        size_type pos;
        for (pos = from_pos; GetRChildPos(pos) != NPOS; pos = GetRChildPos(pos))
            ;
        return pos;
    }

    size_type FindNextBigPos(size_type pos) const {
        if (pos == NPOS) {
            return FindSmallestPos();
        }

        if (GetRChildPos(pos) != NPOS) {
            return FindSmallestPos(GetRChildPos(pos));
        }

        while (GetParentPos(pos) != NPOS) {
            if (GetLChildPos(GetParentPos(pos)) == pos) {
                return GetParentPos(pos);
            }
            pos = GetParentPos(pos);
        }

        return NPOS;
    }

    size_type FindNextSmallPos(size_type pos) const {
        if (pos == NPOS) {
            return FindBiggestPos();
        }

        if (GetLChildPos(pos) != NPOS) {
            return FindBiggestPos(GetLChildPos(pos));
        }

        while (GetParentPos(pos) != NPOS) {
            if (GetRChildPos(GetParentPos(pos)) == pos) {
                return GetParentPos(pos);
            }
            pos = GetParentPos(pos);
        }

        return NPOS;
    }

    //////////////////////////////////////////////////
    // Rotate
    //////////////////////////////////////////////////
    //        parent             parent             |
    //          |                  |                |
    //         pos               lpos               |
    //        /   \             /    \              |
    //     lpos   rpos  ==>  llpos   pos            |
    //     /  \                     /   \           |
    // llpos  lrpos             lrpos    rpos       |
    void RotateRight(size_type pos) {
        size_type parent = GetParentPos(pos);
        size_type lpos = GetLChildPos(pos);
        size_type lrpos = GetRChildPos(lpos);

        // handle parent
        if (parent == NPOS) {
            _root_pos = lpos;
        } else if (pos == GetLChildPos(parent)) {
            SetLChildPos(parent, lpos);
        } else {
            SetRChildPos(parent, lpos);
        }

        // handle lpos
        SetParentPos(lpos, parent);
        SetRChildPos(lpos, pos);

        // handle pos
        SetParentPos(pos, lpos);
        SetLChildPos(pos, lrpos);

        // handle lrpos
        if (lrpos != NPOS) SetParentPos(lrpos, pos);
    }

    //        parent                parent          |
    //          |                      |            |
    //         pos                   rpos           |
    //        /   \                 /    \          |
    //     lpos   rpos    ==>     pos    rrpos      |
    //           /    \          /   \              |
    //       rlpos    rrpos   lpos   rlpos          |
    void RotateLeft(size_type pos) {
        size_type parent = GetParentPos(pos);
        size_type rpos = GetRChildPos(pos);
        size_type rlpos = GetLChildPos(rpos);

        // handle parent
        if (parent == NPOS) {
            _root_pos = rpos;
        } else if (pos == GetLChildPos(parent)) {
            SetLChildPos(parent, rpos);
        } else {
            SetRChildPos(parent, rpos);
        }

        // handle rpos
        SetParentPos(rpos, parent);
        SetLChildPos(rpos, pos);

        // handle pos
        SetParentPos(pos, rpos);
        SetRChildPos(pos, rlpos);

        // handle rlpos
        if (rlpos != NPOS) SetParentPos(rlpos, pos);
    }

    //////////////////////////////////////////////////
    // GetNewNodeAndInsert
    //////////////////////////////////////////////////
    size_type GetNewNodeAndInsert(value_type const &val) {
        std::pair<size_type, size_type> res_pair = FindNode(val);

        size_type pos = res_pair.first;
        size_type parent_pos = res_pair.second;

        // if already exist
        if (pos != NPOS) return NPOS;

        // get one node from empty list
        _size++;
        size_type new_pos = GetNodeFromEmptyList();

        // initiate node
        SetLChildPos(new_pos, NPOS);
        SetRChildPos(new_pos, NPOS);
        SetParentPos(new_pos, parent_pos);
        SetColor(new_pos, RED);

        // this is the first node
        if (parent_pos == NPOS) {
            _root_pos = new_pos;
        } else if (*GetDataPtr(parent_pos) < val) {
            SetRChildPos(parent_pos, new_pos);
        } else {
            SetLChildPos(parent_pos, new_pos);
        }

        // do fixup, maintain rb tree requirements
        InsertFixup(new_pos);
        return new_pos;
    }

    void InsertFixup(size_type new_pos) {
        size_type parent_pos = GetParentPos(new_pos);

        // [1] if node is root, then change it to black
        if (parent_pos == NPOS) {
            SetColor(new_pos, BLACK);
            return;
        }

        // [2] if parent node is black, all features preserved, so return
        if (GetColor(parent_pos) == BLACK) {
            return;
        }

        size_type grand_parent_pos = GetParentPos(parent_pos);
        size_type uncle_pos = GetBrotherPos(parent_pos);

        // [3] if uncle and parent are both red,
        // we change uncle and parent to black, then change grand_parent to red
        // and do it recursively to grand_parent
        //     grandparent[B]             grandparent[R]        |
        //       /       \                 /         \          |
        //   parent[R]  uncle[R]  ==>  parent[B]    uncle[B]    |
        //     /                         /                      |
        //   new[R]                   new[R]                    |
        if (GetColor(uncle_pos) == RED) {
            SetColor(uncle_pos, BLACK);
            SetColor(parent_pos, BLACK);
            SetColor(grand_parent_pos, RED);
            InsertFixup(grand_parent_pos);
            return;
        }

        // [4.1] if new node is rchild of parent, and parent is lchild of grandparent, then we rotate left parent
        //     grandparent[B]           grandparent[B]  |
        //       /        \                /      \     |
        //    parent[R]  uncle[B]  ==>  new[R]  uncle[B]|
        //         \                    /               |
        //          new[R]            parent[R]         |
        if (GetRChildPos(parent_pos) == new_pos && GetLChildPos(grand_parent_pos) == parent_pos) {
            RotateLeft(parent_pos);
            InsertFixup(parent_pos);
            return;
        }

        // [4.2] if new node is lchild of parent, and parent is rchild of grandparent, then we rotate right parent
        //     grandparent[B]          grandparent[B]   |
        //      /      \               /        \       |
        //  uncle[B]  parent[R]  ==> uncle[B]  new[R]   |
        //            /                          \      |
        //          new[R]                   parent[R]  |
        if (GetRChildPos(parent_pos) == new_pos && GetLChildPos(grand_parent_pos) == parent_pos) {
            RotateRight(parent_pos);
            InsertFixup(parent_pos);
            return;
        }

        // [5] make parent black and grandparent red, then rotate grandparent
        //         grandparent[B]                  parent[B]                |
        //          /      \                       /      \                 |
        //     parent[R]   uncle[B]  ==>       new[R]   grandparent[R]      |
        //        /                                         \               |
        //   new[R]                                         uncle[B]        |
        SetColor(parent_pos, BLACK);
        SetColor(grand_parent_pos, RED);

        if (parent_pos == GetLChildPos(grand_parent_pos)) {
            RotateRight(grand_parent_pos);
        } else {
            RotateLeft(grand_parent_pos);
        }
    }

    //////////////////////////////////////////////////
    // EraseNode
    //////////////////////////////////////////////////
    void EraseNode(std::pair<size_type, size_type> pos_pair) {
        _size--;
        size_type pos = pos_pair.first;
        size_type parent_pos = pos_pair.second;

        size_type lchild_pos = GetLChildPos(pos);
        size_type rchild_pos = GetRChildPos(pos);
        // [1] if pos has two child, we must make sure it has at most 1 child
        //            pos               pos             |
        //           /   \              /  \            |
        //      lchild  rchild    lbiggest rchild       |
        //         \                  /                 |
        //         ...        ==>    ...                |
        //          \                   \               |
        //       lbiggest             lbiggest_lchild   |
        //             /                                |
        //      lbiggest_lchild                         |
        if (lchild_pos != NPOS && rchild_pos != NPOS) {
            // find the bigges node in left tree
            size_type lbiggest_pos = FindBiggestPos(lchild_pos);

            // replace node's data with left_biggest node's data
            *GetDataPtr(pos) = *GetDataPtr(lbiggest_pos);

            // then we erase left_biggest node, instead of erase pos
            // becuase left_biggest can't have rchild, this fits our request
            pos = lbiggest_pos;
            parent_pos = GetParentPos(pos);
            lchild_pos = GetLChildPos(pos);
            rchild_pos = GetRChildPos(pos);
        }

        // [2.1] if pos is red, we just erase pos, everything is fine
        //          parent[B]          parent[B]       |
        //             |                  |            |
        //           pos[R]    ==>    lchild[B]        |
        //           /                                 |
        //    lchild[B]                                |
        if (GetColor(pos) == RED && lchild_pos != NPOS) {
            DeleteAndFreeNode(pos, lchild_pos);
            return;
        }
        if (GetColor(pos) == RED && rchild_pos != NPOS) {
            DeleteAndFreeNode(pos, rchild_pos);
            return;
        }
        if (GetColor(pos) == RED && lchild_pos == NPOS && rchild_pos == NPOS) {
            DeleteAndFreeNode(pos, NPOS);
            return;
        }

        // [2.2] if pos has one child, pos is black and child is red, we just erase pos, change child to black
        //          parent[]          parent[]         |
        //             |                  |            |
        //           pos[B]    ==>    lchild[B]        |
        //           /                                 |
        //    lchild[R]                                |
        if (GetColor(pos) == BLACK && GetColor(lchild_pos) == RED) {
            SetColor(lchild_pos, BLACK);
            DeleteAndFreeNode(pos, lchild_pos);
            return;
        }
        if (GetColor(pos) == BLACK && GetColor(rchild_pos) == RED) {
            SetColor(rchild_pos, BLACK);
            DeleteAndFreeNode(pos, rchild_pos);
            return;
        }

        // [2.3] if pos has one child, pos and child are both black, we just erase pos and do fixup
        //          parent[]          parent[]         |
        //             |                  |            |
        //           pos[B]    ==>    lchild[B]        |
        //           /                                 |
        //    lchild[B]                                |
        if (GetColor(pos) == BLACK && lchild_pos != NPOS) {
            DeleteAndFreeNode(pos, lchild_pos);
            EraseFixup(parent_pos, lchild_pos);
            return;
        }
        if (GetColor(pos) == BLACK && rchild_pos != NPOS) {
            DeleteAndFreeNode(pos, rchild_pos);
            EraseFixup(parent_pos, rchild_pos);
            return;
        }
        if (GetColor(pos) == BLACK && lchild_pos == NPOS && rchild_pos == NPOS) {
            DeleteAndFreeNode(pos, NPOS);
            EraseFixup(parent_pos, NPOS);
            return;
        }
    }

    void DeleteAndFreeNode(size_type pos, size_type replace_pos) {
        if (pos == NPOS) return;
        size_type parent_pos = GetParentPos(pos);

        // do delete
        if (parent_pos == NPOS) {
            _root_pos = replace_pos;
        } else if (GetLChildPos(parent_pos) == pos) {
            SetLChildPos(parent_pos, replace_pos);
        } else {
            SetRChildPos(parent_pos, replace_pos);
        }

        if (replace_pos != NPOS) {
            SetParentPos(replace_pos, parent_pos);
        }

        // free pos
        InsertNodeToEmptyList(pos);
    }

    void EraseFixup(size_type parent_pos, size_type pos) {
        // if parent is NPOS, so the pos is root, and there will be no problem
        if (parent_pos == NPOS) return;

        // note that if we comes to EraseFixup, pos must be black
        size_type brother_pos = GetBrotherPos(parent_pos, pos);
        size_type blchild_pos = GetLChildPos(brother_pos);
        size_type brchild_pos = GetRChildPos(brother_pos);

        // [1] if brother is NPOS, just delete
        // [1] if brother is red, here change status to 3-5
        //       parent[B]                             brother[B]           |
        //       /    \                                 /       \           |
        //    pos[B]   brother[R]        ==>       parent[R]   brchild[B]   |
        //            /        \                    /     \                 |
        //        blchild[B]  brchild[B]        pos[B]   blchild[B]         |
        if (GetColor(brother_pos) == RED && GetLChildPos(parent_pos) == pos) {
            SetColor(parent_pos, RED);
            SetColor(brother_pos, BLACK);
            RotateLeft(parent_pos);
            // re-calc brother pos
            brother_pos = GetBrotherPos(parent_pos, pos);
            blchild_pos = GetLChildPos(brother_pos);
            brchild_pos = GetRChildPos(brother_pos);
        }

        if (GetColor(brother_pos) == RED && GetRChildPos(parent_pos) == pos) {
            SetColor(parent_pos, RED);
            SetColor(brother_pos, BLACK);
            RotateRight(parent_pos);
            // re-calc brother pos
            brother_pos = GetBrotherPos(parent_pos, pos);
            blchild_pos = GetLChildPos(brother_pos);
            brchild_pos = GetRChildPos(brother_pos);
        }

        // [2] if parent, brother, blchild, brchild are black
        //       parent[B]                             parent[B]                |
        //       /    \                                 /       \               |
        //    pos[B]   brother[B]        ==>       pos[B]   brother[R]          |
        //            /        \                             /     \            |
        //       blchild[B]   brchild[B]             blchild[B]   blchild[B]    |
        if (GetColor(parent_pos) == BLACK && GetColor(brother_pos) == BLACK && GetColor(blchild_pos) == BLACK &&
            GetColor(brchild_pos) == BLACK) {
            if (brother_pos != NPOS) SetColor(brother_pos, RED);
            EraseFixup(GetParentPos(parent_pos), parent_pos);
            return;
        }

        // [3] if parent is red, but brother, blchild, brchild are black
        //       parent[R]                             parent[B]                |
        //       /    \                                 /       \               |
        //    pos[B]   brother[B]        ==>       pos[B]   brother[R]          |
        //            /        \                             /     \            |
        //       blchild[B]   brchild[B]             blchild[B]   blchild[B]    |
        if (GetColor(parent_pos) == RED && GetColor(brother_pos) == BLACK && GetColor(blchild_pos) == BLACK &&
            GetColor(brchild_pos) == BLACK) {
            SetColor(parent_pos, BLACK);
            if (brother_pos != NPOS) SetColor(brother_pos, RED);
            return;
        }

        // [4] if blchild is red, brother and brchild are black,
        //       parent[]                         parent[]                  |
        //       /    \                             /     \                 |
        //    pos[B]   brother[B]        ==>   pos[B]   blchild[B]          |
        //            /        \                         /      \           |
        //       blchild[R]   brchild[B]                [B]    brother[R]   |
        //          /                                              \        |
        //        [B]                                           brchild[B]  |
        if (GetLChildPos(parent_pos) == pos && GetColor(blchild_pos) == RED && GetColor(brchild_pos) == BLACK) {
            SetColor(brother_pos, RED);
            SetColor(blchild_pos, BLACK);
            RotateRight(brother_pos);
            // re-calc brother pos
            brother_pos = GetBrotherPos(parent_pos, pos);
            blchild_pos = GetLChildPos(brother_pos);
            brchild_pos = GetRChildPos(brother_pos);
        }

        if (GetRChildPos(parent_pos) == pos && GetColor(blchild_pos) == BLACK && GetColor(brchild_pos) == RED) {
            SetColor(brother_pos, RED);
            SetColor(brchild_pos, BLACK);
            RotateLeft(brother_pos);
            // re-calc brother pos
            brother_pos = GetBrotherPos(parent_pos, pos);
            blchild_pos = GetLChildPos(brother_pos);
            brchild_pos = GetRChildPos(brother_pos);
        }

        // [5] if brchild is red, brother is black
        //       parent[]                              brother[]                |
        //       /    \                                 /     \                 |
        //    pos[B]   brother[B]        ==>      parent[B]   brchild[B]        |
        //            /        \                  /      \                      |
        //       blchild[]   brchild[R]      pos[B]   blchild[]                 |
        if (GetLChildPos(parent_pos) == pos && GetColor(brchild_pos) == RED) {
            SetColor(brother_pos, GetColor(parent_pos));
            SetColor(parent_pos, BLACK);
            SetColor(brchild_pos, BLACK);
            RotateLeft(parent_pos);
        }

        //         parent[]                brother[]            |
        //          /    \                  /      \            |
        //    brother[B] pos[B]  ==>  blchild[B]   parent[B]    |
        //     /       \                           /     \      |
        // blchild[R] brchild[]             brchild[]  pos[B]   |
        if (GetRChildPos(parent_pos) == pos && GetColor(blchild_pos) == RED) {
            SetColor(brother_pos, GetColor(parent_pos));
            SetColor(parent_pos, BLACK);
            SetColor(blchild_pos, BLACK);
            RotateRight(parent_pos);
        }
    }

    ////////////////////////////////////////////////////////////
    // other funcs
    ////////////////////////////////////////////////////////////
    // get one node from the empty list, return the pos
    size_type GetNodeFromEmptyList() {
        size_type pos = _empty_head_pos;
        if (GetRChildPos(_empty_head_pos) == _empty_head_pos) {  // single node loop
            _empty_head_pos = NPOS;
        } else {
            SetLChildPos(GetRChildPos(_empty_head_pos), GetLChildPos(_empty_head_pos));
            SetRChildPos(GetLChildPos(_empty_head_pos), GetRChildPos(_empty_head_pos));
            _empty_head_pos = GetRChildPos(_empty_head_pos);
        }
        return pos;
    }

    // insert pos before the empty_head_pos
    void InsertNodeToEmptyList(size_type pos) {
        if (_empty_head_pos == NPOS) {  // none node, so form a sinlge loop node
            SetRChildPos(pos, pos);
            SetLChildPos(pos, pos);
        } else {
            SetRChildPos(pos, _empty_head_pos);
            SetLChildPos(pos, GetLChildPos(_empty_head_pos));
            SetLChildPos(GetRChildPos(pos), pos);
            SetRChildPos(GetLChildPos(pos), pos);
        }
        SetParentPos(pos, NPOS);
        SetColor(pos, EMPTY);
        _empty_head_pos = pos;
    }

    ////////////////////////////////////////////////////////////
    // accessor funcs
    ////////////////////////////////////////////////////////////
    bool full() const { return _size == _max_size - 1; }
    bool empty() const { return _size == 0; }

    size_type size() const { return _size; }
    size_type max_size() const { return _max_size - 1; }
    size_t ElemByteSize() const { return _elem_byte_size; }

    node_type *GetNodePtr(size_type pos) { return (node_type *)(this + 1) + pos; }
    value_type *GetDataPtr(size_type pos) { return &GetNodePtr(pos)->data; }

    node_type const *GetNodePtr(size_type pos) const { return (node_type *)(this + 1) + pos; }
    value_type const *GetDataPtr(size_type pos) const { return &GetNodePtr(pos)->data; }

    size_type GetRootPos() const { return _root_pos; }
    size_type GetLChildPos(size_type pos) const {
        if (pos == NPOS) return NPOS;
        return GetNodePtr(pos)->lchild_pos;
    }
    size_type GetRChildPos(size_type pos) const {
        if (pos == NPOS) return NPOS;
        return GetNodePtr(pos)->rchild_pos;
    }
    size_type GetParentPos(size_type pos) const { return GetNodePtr(pos)->parent_pos; }
    uint8_t GetColor(size_type pos) const { return GetNodePtr(pos)->color; }

    void SetLChildPos(size_type pos, size_type lchild_pos) { GetNodePtr(pos)->lchild_pos = lchild_pos; }
    void SetRChildPos(size_type pos, size_type rchild_pos) { GetNodePtr(pos)->rchild_pos = rchild_pos; }
    void SetParentPos(size_type pos, size_type parent_pos) { GetNodePtr(pos)->parent_pos = parent_pos; }
    void SetColor(size_type pos, uint8_t color) { GetNodePtr(pos)->color = color; }

    // this version pos cant be NPOS
    size_type GetBrotherPos(size_type pos) const {
        if (pos == NPOS) return NPOS;

        size_type parent_pos = GetParentPos(pos);
        if (GetLChildPos(parent_pos) == pos)
            return GetRChildPos(parent_pos);
        else
            return GetLChildPos(parent_pos);
    }

    size_type GetBrotherPos(size_type parent_pos, size_type pos) const {
        if (pos == NPOS) {
            if (GetLChildPos(parent_pos) == NPOS)
                return GetRChildPos(parent_pos);
            else
                return GetLChildPos(parent_pos);
        } else {
            return GetBrotherPos(pos);
        }
    }

    static size_t CalcByteSize(size_type size) { return (size + 1) * sizeof(node_type) + sizeof(this_type); }
    static size_t CalcElemByteSize(void const *ptr) { return ((this_type const *)ptr)->ElemByteSize(); }
    static size_t CalcElemMaxSize(void const *ptr) { return ((this_type const *)ptr)->max_size(); }

private:
    size_type _size;
    size_type _max_size;
    size_type _elem_byte_size;
    // here we use one NIL Node to mark all nil leaf node
    // so must store this nil pos in state
    size_type _nil_pos;
    size_type _root_pos;
    size_type _empty_head_pos;
};

////////////////////////////////////////////////////////////
// RBTreeIterator
////////////////////////////////////////////////////////////
template <typename ElemType, typename SizeType>
class RBTreeIterator : public std::iterator<std::bidirectional_iterator_tag, ElemType> {
public:
    // typedef
    typedef std::iterator<std::bidirectional_iterator_tag, ElemType> base_type;
    typedef typename base_type::value_type value_type;
    typedef typename base_type::pointer pointer;
    typedef typename base_type::reference reference;
    typedef typename base_type::difference_type  difference_type;
    typedef RBTreeState<ElemType, SizeType> state_type;
    typedef SizeType size_type;

public:
    // constructor
    RBTreeIterator(state_type *ptr_state = nullptr, size_type pos = 0) : _ptr_state(ptr_state), _pos(pos) {}

    // member funcs
    RBTreeIterator operator+(difference_type diff) const {
        RBTreeIterator tmp = *this;

        if (diff < 0) return tmp - (-diff);
        while (diff != 0) {
            ++tmp;
            --diff;
        }
        return tmp;
    }

    RBTreeIterator operator-(difference_type diff) const {
        RBTreeIterator tmp = *this;

        if (diff < 0) return tmp + (-diff);
        while (diff != 0) {
            --tmp;
            --diff;
        }
        return tmp;
    }

    RBTreeIterator &operator++() {
        _pos = _ptr_state->FindNextBigPos(_pos);
        return *this;
    }
    RBTreeIterator &operator--() {
        _pos = _ptr_state->FindNextSmallPos(_pos);
        return *this;
    }

    pointer operator->() { return _ptr_state->GetDataPtr(_pos); }
    reference operator*() { return *(_ptr_state->GetDataPtr(_pos)); }

    RBTreeIterator operator++(int) {
        RBTreeIterator tmp = *this;
        ++*this;
        return tmp;
    }
    RBTreeIterator operator--(int) {
        RBTreeIterator tmp = *this;
        --*this;
        return tmp;
    }

    bool operator==(RBTreeIterator const &rhs) const { return _ptr_state == rhs._ptr_state && _pos == rhs._pos; }
    bool operator!=(RBTreeIterator const &rhs) const { return !(_ptr_state == rhs._ptr_state && _pos == rhs._pos); }

    size_type GetPos() const { return _pos; }

private:
    state_type *_ptr_state;
    size_type _pos;
};

}  // end of namespace detail
}  // end of namespace container
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
