#pragma once

#include <string>
#include <deque>
#include <memory>
#include "json.h"

namespace json
{
    class BaseItemContext;
    class KeyContext;
    class ValueContext;
    class StartDictContext;
    class StartArrayContext;
    class EndContext;
    class DictValueContext;
    class ArrayValueContext;

    class Builder
    {
    public:
        KeyContext Key(std::string key);
        ValueContext Value(Node value);
        StartArrayContext StartArray();
        EndContext EndArray();
        StartDictContext StartDict();
        EndContext EndDict();
        Node Build();

    private:
        std::unique_ptr<Node> root_;
        std::deque<std::unique_ptr<Node>> nodes_stack_;
    };

    class BaseItemContext
    {
    public:
        BaseItemContext() = delete;
        BaseItemContext(Builder& builder);

    protected:
        Builder& builder_;
    };

    class KeyContext : public BaseItemContext
    {
    public:
        KeyContext(Builder& builder);
        DictValueContext Value(Node value);
        StartDictContext StartDict();
        StartArrayContext StartArray();
    };

    class ValueContext : public BaseItemContext
    {
    public:
        ValueContext(Builder& builder);
        Node Build();
    };

    class DictValueContext : public BaseItemContext
    {
    public:
        DictValueContext(Builder& builder);
        KeyContext Key(std::string key);
        EndContext EndDict();
    };

    class ArrayValueContext : public BaseItemContext
    {
    public:
        ArrayValueContext(Builder& builder);
        StartDictContext StartDict();
        ArrayValueContext& Value(Node value); 
        EndContext EndArray(); 
    };

    class StartDictContext : public BaseItemContext
    {
    public:
        StartDictContext(Builder& builder);
        KeyContext Key(std::string key); 
        EndContext EndDict(); 
    };

    class StartArrayContext : public BaseItemContext
    {
    public:
        StartArrayContext(Builder& builder);
        ArrayValueContext Value(Node value); 
        StartDictContext StartDict();
        StartArrayContext StartArray();
        EndContext EndArray(); 
    };

    class EndContext : public BaseItemContext
    {
    public:
        EndContext(Builder& builder);
        KeyContext Key(std::string key);
        StartDictContext StartDict();
        StartArrayContext StartArray();
        EndContext& EndDict();
        EndContext& EndArray();
        ArrayValueContext Value(Node value);
        Node Build();
    };
}