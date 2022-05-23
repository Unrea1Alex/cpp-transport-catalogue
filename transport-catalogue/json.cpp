#include "json.h"

using namespace std;

namespace json 
{

    Node::Node(Array array) : value_(move(array)) { }

    Node::Node(Dict map) : value_(move(map)) { }

    Node::Node(int value) : value_(value) { }

    Node::Node(double value) : value_(value) { }

    Node::Node(string value) : value_(move(value)) { }

    Node::Node(bool value) : value_(value) {}

    Node::Node(std::nullptr_t value) : value_(value) {}

    Node::Node() : value_(nullptr) {}

    const Array& Node::AsArray() const 
    {
        return IsArray() ? std::get<Array>(value_) : throw std::logic_error("not array");
    }

    const Dict& Node::AsMap() const 
    {
        return IsMap() ? std::get<Dict>(value_) : throw std::logic_error("not array");
    }

    int Node::AsInt() const 
    {
        return IsInt() ? std::get<int>(value_) : throw std::logic_error("not array");
    }

    bool Node::AsBool() const 
    {
        return IsBool() ? std::get<bool>(value_) : throw std::logic_error("not array");
    }

    double Node::AsDouble() const 
    {
        return IsDouble() ? (IsInt() ? (double)std::get<int>(value_) : std::get<double>(value_)) : throw std::logic_error("not array");
    }

    const string& Node::AsString() const 
    {
        return IsString() ? std::get<std::string>(value_) : throw std::logic_error("not array");
    }

    Value Node::GetValue() const
    {
        return value_;
    }

    bool Node::operator==(const Node& other) const
    {
        return value_ == other.value_;
    }

    bool Node::operator!=(const Node& other) const
    {
        return !(*this == other);
    }

    Document::Document(Node root) : root_(move(root)) { }

    Document::Document(Array arr)
    {
        root_ = Node(std::move(arr));
    }

    const Node& Document::GetRoot() const 
    {
        return root_;
    }

    bool Document::operator==(const Document& other) const
    {
        return root_ == other.root_;
    }

    bool Document::operator!=(const Document& other) const
    {
        return !(*this == other);
    }

}