#include <string>
#include <stdexcept>
#include <iostream>
#include <vector>

constexpr int LEVEL = 0;
constexpr int  REVERSE = 1;
constexpr int  PRE = 2 ;

#define DEBUG 0
namespace ariel
{
    class OrgChart
    {
        public:
        OrgChart() : first(nullptr)
        {

        }

        OrgChart(const OrgChart& ot)
        {
            first = new Node(ot.first);
        }

        // Move ctor, allows a resources to be shifted without creating a copy.
        OrgChart(OrgChart&& ot) noexcept // delcaration that the function will not throw an exception
        {
            first = ot.first;
            ot.first = nullptr;
        }

        private:
            struct Node 
            {
                std::string value;
                std::vector<Node*> children;

                /*Parameters to help iteration*/
                Node* parent;
                bool visited;
                Node(const std::string& val) 
                : value(val), parent(nullptr), visited(false)
                {
                }
                Node(const Node* ot)
                : value(ot->value), parent(nullptr), visited(false)
                // To ensure deep copy of children vector, define new node for each child, and do the same for the children.
                {
                    initNode(this, ot);
                }

                void initNode(Node* curr, const Node* ot)
                {
                    for (Node* child : ot->children)
                    {
                        Node* c = new Node(child->value);
                        c->parent = curr;
                        curr->children.push_back(c);
                        initNode(c, child);
                    }
                }
            };
        
        Node* first;

        /**
         * @brief Frees all node pointers in the tree.
         * 
         * @param curr 
         */
        void clear(Node* curr);
        
        /**
         * @brief Finds node in tree by given string, else returns null pointer.
         * 
         * @param val 
         * @param curr 
         * @return Node* 
         */
        Node* find(const std::string& val, Node* curr);

        public:
        ~OrgChart()
        {
            clear(first);
        }

        OrgChart& operator=(const OrgChart& ot)
        {

            if (this == &ot)
            {
                return *this;
            }

            // Delete current, deep copy from other to self, return self.

            clear(first);
            if (ot.first != nullptr)
            {
                first = new Node(ot.first);
            }
            return *this;
        }

        OrgChart& operator=(OrgChart&& ot) noexcept // delcaration that the function will not throw an exception
        {
            if (this == &ot)
            {
                return *this;
            }
            // Move resources without creating additional copies

            first = ot.first;
            ot.first = nullptr;
            return *this;
        }

        /**
         * @brief Adds new root with a given string, if there is already a root, replaces his data.
         * 
         * @param r 
         * @return OrgChart& 
         */
        OrgChart& add_root(const std::string& r);

        /**
         * @brief Finds a node by a given string, and creates a child node.
         * 
         * @param dad_name 
         * @param name 
         * @return OrgChart& 
         */
        OrgChart& add_sub(const std::string& dad_name, const std::string& name);

        /* Prints current OrgTree horizontally
        A ----> B-->D
                |   |
                |   E
                C

            Same as:
                
                A
             B     C
           D--E
        With help of https://stackoverflow.com/questions/36802354/print-binary-tree-in-a-pretty-way-using-c
        */
        void show(Node* n, const std::string& prefix, bool isLast) const;

        friend std::ostream& operator << (std::ostream& out, const OrgChart& ot)
        {
            ot.show(ot.first, "", false);
            return out;
        }
        friend std::ostream& operator << (std::ostream& out, const OrgChart* ot)
        {
            ot->show(ot->first, "", false);
            return out;
        }

        class iterator
        {
          private:
          Node* ptr_curr;
          Node* root;
          int mode;

        // https://www.geeksforgeeks.org/find-deepest-node-binary-tree/
        // implemented iterations with help of link above
        /**
         * @brief Resets all of the nodes visited status.
         * 
         * @param curr 
         */
        void mark_all_unvisited(Node* curr);

        /**
         * @brief Returns the left-most unvisited node. [Preorder]
         * 
         * @param curr 
         * @return Node* 
         */

        Node* find_unvisited_left_most(Node* curr);

        /**
         * @brief Returns the left-most and highest unvisited node. [Level]
         * 
         * @param curr 
         * @param level 
         * @param minLevel 
         * @param ans 
         */
        void find_shallowest_left_child_rec(Node* curr, int level, int &minLevel, Node** ans);

        /**
         * @brief Returns the left-most and highest unvisited node, by calling the function above [Level]
         * 
         * @return Node* 
         */
        Node* find_shallowest_left_child();

        /**
         * @brief Given a current node, returns the next unvisited node by level order.
         * 
         * @param curr 
         * @return Node* 
         */
        Node* find_next_level(Node* curr);
        
        /**
         * @brief Returns the left-most, deepest node. [Reverse]
         * 
         * @param curr 
         * @param level 
         * @param maxLevel 
         * @param ans 
         */
        void get_deepest_left(Node* curr, int level, int &maxLevel, Node** ans);

        /**
         * @brief Returns the left-most, deepest node. [Reverse]
         * 
         * @return Node* 
         */
        Node* find_next_reverse();

          public:
          iterator(Node* ptr = nullptr, int m = 0) 
          : ptr_curr(ptr), root(ptr), mode(m)
          {
              if (ptr_curr != nullptr)
              {
                  mark_all_unvisited(ptr_curr);
                  
                  if (m != REVERSE)
                  {
                    ptr_curr->visited = true;
                  }
                  else
                  {
                      ptr_curr = find_next_reverse();
                      ptr_curr->visited = true;
                  }
              }
          }  
          std::string& operator*()
          {
              return ptr_curr->value;
          }
          std::string* operator->()
          {
              return &(ptr_curr->value);
          }
          iterator& operator++()
          {
              /*    A
                  B   C
                D
              */
             if (ptr_curr == nullptr)
                {
                    return *this;
                }
              if (mode == LEVEL) // A B C D
              {
                  ptr_curr = find_next_level(ptr_curr);
              }
              if (mode == PRE) // A B D C
              {
                  ptr_curr = find_unvisited_left_most(ptr_curr);
              }
              if (mode == REVERSE) // D B C A 
              {
                  ptr_curr = find_next_reverse();
              }
              return *this;
          }
          iterator operator++(int)
          {
              iterator temp = *this;
              operator++();
              return temp;
          }
          bool operator==(const iterator& other) const
          {
              return ptr_curr == other.ptr_curr;
          }
          bool operator!=(const iterator& other) const
          {
              
              return this->ptr_curr != other.ptr_curr;
          }
        }; // ITERATOR END

          void isValid()
          {
            if (first == nullptr)
            {
                throw std::invalid_argument("Can't iterate empty org.");
            }
          }

        // Default iterator, returns level
        iterator begin()
        {
            isValid();
            return iterator{first, LEVEL};
        }
        iterator end()
        {
            isValid();
            return iterator{nullptr, LEVEL};
        }

        iterator begin_level_order()
        {
            isValid();
            return iterator{first, LEVEL};
        }
        iterator end_level_order()
        {
            isValid();
            return iterator{nullptr, LEVEL};
        }
        iterator begin_reverse_order()
        {
            isValid();
            return iterator{first, REVERSE};
        }
        iterator reverse_order()
        {
            isValid();
            return iterator{nullptr, REVERSE};
        }
        iterator begin_preorder()
        {
            isValid();
            return iterator{first, PRE};
        }
        iterator end_preorder()
        {
            isValid();
            return iterator{nullptr, PRE};
        }
    };
}
