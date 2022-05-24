#include "OrgChart.hpp"
using namespace ariel;
constexpr int MAX_INT = 2147483647;

void OrgChart::clear(Node* curr)
{
    if (curr != nullptr)
    {
        for (Node* child : curr->children)
        {
            clear(child);
        }
        delete curr;
    }
}

/**
 * @brief Finds node in tree by given string, else returns null pointer.
 * 
 * @param val 
 * @param curr 
 * @return Node* 
 */
OrgChart::Node* OrgChart::find(const std::string& val, Node* curr)
{
    if (curr != nullptr)
    {
        if (curr->value == val)
        {
            return curr;
        }

        if (curr->children.empty())
        {
            return nullptr;
        }

        for (Node* n : curr->children)
        {
            Node* a = find(val, n);
            if (a != nullptr)
            {
                return a;
            }
        }
    }
    return nullptr;
}

/**
 * @brief Adds new root with a given string, if there is already a root, replaces his data.
 * 
 * @param r 
 * @return OrgChart& 
 */
OrgChart& OrgChart::add_root(const std::string& r)
{
    if (first != nullptr)
    {
        first->value = r;
    }
    else
    {
        first = new Node(r);
    }
    return *this;
}

/**
 * @brief Finds a node by a given string, and creates a child node.
 * 
 * @param dad_name 
 * @param name 
 * @return OrgChart& 
 */
OrgChart& OrgChart::add_sub(const std::string& dad_name, const std::string& name)
{
    /*Check dad is in chart*/
    Node* sub = find(dad_name, first);
    if (sub == nullptr)
    {
        if (DEBUG)
        {
            std::cout << "Did not find node:"<< dad_name << std::endl;
        }
        throw std::invalid_argument("add_sub(): left parameter node not in chart.");
    }

    Node* newborn = new Node(name);
    newborn->parent = sub;
    sub->children.push_back(newborn);
    return *this;
}

void OrgChart::show(Node* n, const std::string& prefix, bool isLast) const
{
    if (n != nullptr)
    {
        std::cout << prefix;
        if (isLast)
        {
            std::cout << "└── ";
        }
        else
        {        
            std::cout << "├── ";
        }
        std::cout << n->value << std::endl;

        for (size_t i = 0; i < n->children.size(); i++)
        {
            if (i + 1 == n->children.size()) // LAST
            {
                show(n->children.at(i), prefix +("│   "), true);
            }
            else
            {
                show(n->children.at(i), prefix +("│   "), false);

            }
        }
    }
}

/*Iterator functions*/
/**
 * @brief Resets all of the nodes visited status.
 * 
 * @param curr 
 */
void OrgChart::iterator::mark_all_unvisited(OrgChart::Node* curr)
{
    if (curr != nullptr)
    {
        curr->visited = false;
        for (Node* child : curr->children)
        {
            OrgChart::iterator::mark_all_unvisited(child);
        }
    }
}


OrgChart::Node* OrgChart::iterator::find_unvisited_left_most(OrgChart::Node* curr)
{
    if (curr == nullptr)
    {
        return nullptr;
    }
    for (Node* child : curr->children)
    {
        if (!child->visited)
        {
            child->visited = true;
            return child;
        }
    }
    return OrgChart::iterator::find_unvisited_left_most(curr->parent);
}

/**
 * @brief Returns the left-most and highest unvisited node.
 * 
 * @param curr 
 * @param level 
 * @param minLevel 
 * @param ans 
 */
void OrgChart::iterator::find_shallowest_left_child_rec(OrgChart::Node* curr, int level, int &minLevel, OrgChart::Node** ans)
{
    if (curr != nullptr)
    {
        for (Node* child : curr->children)
        {
            OrgChart::iterator::find_shallowest_left_child_rec(child, level + 1, minLevel, ans);
        }

        if (level < minLevel)
        {
            if (!curr->visited)
            {
                *ans = curr;
                minLevel = level;             
            }
        }
    }
}

/**
 * @brief Returns the left-most and highest unvisited node.
 * 
 * @return Node* 
 */
OrgChart::Node* OrgChart::iterator::find_shallowest_left_child()
{
    Node* ans = nullptr;
    int minLevel = MAX_INT;
    OrgChart::iterator::find_shallowest_left_child_rec(this->root, 0, minLevel, &ans);
    if (ans == nullptr || ans->visited)
    {
        return nullptr;
    }
    ans->visited = true;
    return ans;
}

/**
 * @brief Given a current node, returns the next unvisited node by level order.
 * 
 * @param curr 
 * @return Node* 
 */
OrgChart::Node* OrgChart::iterator::find_next_level(OrgChart::Node* curr)
{
    /* basic idea: 
    • return any untraveled sibling you have
    • else go up the tree and find untraveled children of a node
    • else return nullptr
    */
    if (curr == nullptr)
    {
        return nullptr;
    }

    if (curr->parent == nullptr)
    {
        for (Node* child : curr->children)
        {
            if (!child->visited)
            {
                child->visited = true;
                return child;
            }
        }
    }

    else
    {
        for (Node* sibling : curr->parent->children)
        {
            if (!sibling->visited)
            {
                sibling->visited = true;
                return sibling;
            }
        }
    }

    return OrgChart::iterator::find_shallowest_left_child();
}

/**
 * @brief Returns the left-most, deepest node.
 * 
 * @param curr 
 * @param level 
 * @param maxLevel 
 * @param ans 
 */
void OrgChart::iterator::get_deepest_left(OrgChart::Node* curr, int level, int &maxLevel, OrgChart::Node** ans)
{
    if (curr != nullptr)
    {
        for (Node* child : curr->children)
        {
            OrgChart::iterator::get_deepest_left(child, level + 1, maxLevel, ans);
        }
        if (level > maxLevel && !curr->visited)
        {
            *ans = curr;
            maxLevel = level;
        }
    }
}

/**
 * @brief Returns the left-most, deepest node.
 * 
 * @return Node* 
 */
OrgChart::Node* OrgChart::iterator::find_next_reverse()
{
    /* basic idea: 
    • get the deepest root in the tree, if there are several, get the one to the left
    • mark it as visited and return it
    */
    Node* ans = nullptr;
    int maxLevel = -1;
    OrgChart::iterator::get_deepest_left(this->root, 0, maxLevel, &ans);

    if (ans == nullptr || ans->visited)
    {
        return nullptr;
    }
    ans->visited = true;
    return ans;
}
