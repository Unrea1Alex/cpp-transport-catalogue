#include "json_builder.h"

#include <cassert>

namespace json
{
    KeyContext Builder::Key(std::string key)
    {
        if(nodes_stack_.empty())
        {
            throw std::logic_error("KeyError");
        }

        if(nodes_stack_.back().get()->IsDict())
        {
            nodes_stack_.emplace_back(std::make_unique<Node>(key));
            return KeyContext(*this);
        }
        throw std::logic_error("KeyError");
    }

    ValueContext Builder::Value(Node value)
    {
        if(root_.get() && nodes_stack_.empty())
        {
            throw std::logic_error("Value error");
        }

        if(nodes_stack_.empty())
        {
            root_ = std::make_unique<Node>(value);
            return ValueContext{*this};
        }

        if(nodes_stack_.back().get()->IsString())
        {
            std::string key = nodes_stack_.back().get()->AsString();
            nodes_stack_.pop_back();

            if(nodes_stack_.back().get()->IsDict())
            {
                nodes_stack_.back().get()->AsDict()[key] = value;
            }
            else 
            {
                throw std::logic_error("Value error");
            }
            return ValueContext{*this};
        }

        if(nodes_stack_.back().get()->IsArray())
        {
            nodes_stack_.back().get()->AsArray().emplace_back(value);
            return ValueContext{*this};
        }
        throw std::logic_error("Value error");
    }

    StartArrayContext Builder::StartArray()
    {
        if(root_.get() && nodes_stack_.empty())
        {
            throw std::logic_error("Value error");
        }

        if(nodes_stack_.empty())
        {
            root_ = std::make_unique<Node>(Node(Array{}));
            nodes_stack_.emplace_back(std::move(root_));
            return StartArrayContext(*this);
        }

        if(nodes_stack_.back().get()->IsString() || nodes_stack_.back().get()->IsArray())
        {
            nodes_stack_.emplace_back(std::make_unique<Node>(Array{}));
            return StartArrayContext(*this);
        }
        return StartArrayContext(*this);
    }

    EndContext Builder::EndArray()
    {
        if(nodes_stack_.empty())
        {
            throw std::logic_error("Array error");
        }

        if(nodes_stack_.back().get()->IsArray())
        {
            auto tmp_arr = std::move(nodes_stack_.back());
            nodes_stack_.pop_back();

            if(nodes_stack_.size() == 0)
            {
                root_ = std::make_unique<Node>(*tmp_arr);
                return EndContext(*this);
            }

            if(nodes_stack_.back().get()->IsString() || nodes_stack_.back().get()->IsArray())
            {
                Value(*tmp_arr);
            }
            return EndContext(*this);
        }
        
        throw std::logic_error("Array error");
    }

    json::StartDictContext Builder::StartDict()
    {
        if(root_.get() && nodes_stack_.empty())
        {
            throw std::logic_error("Value error");
        }
        
        if(nodes_stack_.empty())
        {
            root_ = std::make_unique<Node>(Node(Dict{}));
            nodes_stack_.emplace_back(std::move(root_));
            return StartDictContext(*this);
        }

        if(nodes_stack_.back().get()->IsString() || nodes_stack_.back().get()->IsArray())
        {
            nodes_stack_.emplace_back(std::make_unique<Node>(Dict{}));
            return StartDictContext(*this);
        }
        throw std::logic_error("Dict error");
    }

    EndContext Builder::EndDict()
    {
        if(nodes_stack_.empty())
        {
            throw std::logic_error("Dict error");
        }

        if(nodes_stack_.back().get()->IsDict())
        {
            Node* tmp_dict = nodes_stack_.back().release();
            nodes_stack_.pop_back();

            if(nodes_stack_.empty())
            {
                root_ = std::make_unique<Node>(*tmp_dict);
                return EndContext(*this);
            }

            if(nodes_stack_.back().get()->IsString() || nodes_stack_.back().get()->IsArray())
            {
                Value(*tmp_dict);
            }
            return EndContext(*this);
        }
        throw std::logic_error("Dict error");
    }

    Node Builder::Build()
    {
        if(!root_.get())
        {
            throw std::logic_error("error");
        }

        if(nodes_stack_.empty())
        {
            return *root_.get();
        }
        throw std::logic_error("error");
    }

    // BaseItemContext

    BaseItemContext::BaseItemContext(Builder& builder) : builder_(builder) { }

    // Key context

    KeyContext::KeyContext(Builder& builder) : BaseItemContext(builder) { }

    DictValueContext KeyContext::Value(Node value)
    {
        builder_.Value(value);
        return DictValueContext{builder_};
    }

    StartDictContext KeyContext::StartDict()
    {
        builder_.StartDict();
        return StartDictContext{builder_};
    }

    StartArrayContext KeyContext::StartArray()
    {
        builder_.StartArray();
        return StartArrayContext{builder_};
    }

    // Value context

    ValueContext::ValueContext(Builder& builder) : BaseItemContext(builder) { }

    Node ValueContext::Build()
    {
        return builder_.Build();
    }

    DictValueContext::DictValueContext(Builder& builder) : BaseItemContext(builder) { }

    KeyContext DictValueContext::Key(std::string key)
    {
        builder_.Key(key);
        return KeyContext{builder_};
    }

    EndContext DictValueContext::EndDict()
    {
        builder_.EndDict();
        return EndContext{builder_};
    }

    ArrayValueContext::ArrayValueContext(Builder& builder) : BaseItemContext(builder) { }

    StartDictContext ArrayValueContext::StartDict()
    {
        builder_.StartDict();
        return StartDictContext{builder_};
    }

    ArrayValueContext& ArrayValueContext::Value(Node value)
    {
        builder_.Value(value);
        return *this;
    }

    EndContext ArrayValueContext::EndArray()
    {
        builder_.EndArray();
        return EndContext{builder_};
    }

    // StartDict context

    StartDictContext::StartDictContext(Builder& builder) : BaseItemContext(builder) { }

    KeyContext StartDictContext::Key(std::string key)
    {
        builder_.Key(key);
        return KeyContext{builder_};
    }

    EndContext StartDictContext::EndDict()
    {
        builder_.EndDict();
        return EndContext{builder_};
    }

    // StartArray context

    StartArrayContext::StartArrayContext(Builder& builder) : BaseItemContext(builder) { }

    ArrayValueContext StartArrayContext::Value(Node value)
    {
        builder_.Value(value);
        return ArrayValueContext{builder_};
    }

    StartDictContext StartArrayContext::StartDict()
    {
        builder_.StartDict();
        return StartDictContext{builder_};
    }

    StartArrayContext StartArrayContext::StartArray()
    {
        builder_.StartArray();
        return StartArrayContext{builder_};
    }

    EndContext StartArrayContext::EndArray()
    {
        builder_.EndArray();
        return EndContext{builder_};
    }

    // EndContext

    EndContext::EndContext(Builder& builder) : BaseItemContext(builder) { }

    Node EndContext::Build()
    {
        return builder_.Build();
    }

    KeyContext EndContext::Key(std::string key)
    {
        builder_.Key(key);
        return KeyContext{builder_};
    }

    StartDictContext EndContext::StartDict()
    {
        builder_.StartDict();
        return StartDictContext{builder_};
    }

    StartArrayContext EndContext::StartArray()
    {
        builder_.StartArray();
        return StartArrayContext{builder_};
    }

    EndContext& EndContext::EndDict()
    {
        builder_.EndDict();
        return *this;
    }

    EndContext& EndContext::EndArray()
    {
        builder_.EndArray();
        return *this;
    }

    ArrayValueContext EndContext::Value(Node value)
    {
        builder_.Value(value);
        return ArrayValueContext{builder_};
    }
}