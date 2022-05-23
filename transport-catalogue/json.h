#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json
{
    class Node;

    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    class ParsingError : public std::runtime_error 
    {
    public:
        using runtime_error::runtime_error;
    };

    using Value = std::variant<std::nullptr_t, int, double, std::string, bool, Array, Dict>;

    class Node 
    {
    public:

        Node();
        Node(Array array);
        Node(Dict map);
        Node(int value);
        Node(double value);
        Node(std::string value);
        Node(bool value);
        Node(std::nullptr_t value);

        int AsInt() const;
        bool AsBool() const;
        double AsDouble() const; 
        const std::string& AsString() const;
        const Array& AsArray() const;
        const Dict& AsMap() const;

        inline bool IsInt() const { return value_.index() == 1; }
        inline bool IsDouble() const { return value_.index() == 2 || value_.index() == 1; }
        inline bool IsPureDouble() const { return value_.index() == 2; }
        inline bool IsBool() const { return value_.index() == 4; }
        inline bool IsString() const { return value_.index() == 3; }
        inline bool IsNull() const { return value_.index() == 0; }
        inline bool IsArray() const { return value_.index() == 5; }
        inline bool IsMap() const { return value_.index() == 6; }

        Value GetValue() const;

        bool operator==(const Node& other) const;

        bool operator!=(const Node& other) const;

    private:

        Value value_;
    };

    class Document 
    {
    public:

        Document(Node root);
        Document(Array arr);

        const Node& GetRoot() const;

        void ParseTokens(std::istream& stream);

        bool operator==(const Document& other) const;
        bool operator!=(const Document& other) const;

    private:

        Node root_;
    };
}  