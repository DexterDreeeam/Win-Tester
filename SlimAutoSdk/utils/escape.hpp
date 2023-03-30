#pragma once

class escape
{
    class internal_release_base
    {
    public:
        internal_release_base()
        {}

        virtual ~internal_release_base()
        {}

        virtual void disable() = 0;
    };

    template<typename Fn_Ty>
    class internal_release : public internal_release_base
    {
    public:
        internal_release(Fn_Ty fn) :
            _fn(fn),
            _is_active(true)
        {}

        virtual ~internal_release() override
        {
            if (_is_active)
            {
                _fn();
            }
        }

        virtual void disable() override
        {
            _is_active = false;
        }

    private:
        Fn_Ty _fn;
        bool _is_active;
    };

public:
    escape() :
        _release(nullptr)
    {}

    template<typename Fn_Ty>
    escape(Fn_Ty fn) :
        _release(new internal_release<Fn_Ty>(fn))
    {}

    ~escape()
    {
        if (_release)
        {
            delete _release;
        }
    }

    template<typename Fn_Ty>
    escape& operator =(Fn_Ty fn)
    {
        if (_release)
        {
            _release->disable();
            delete _release;
        }
        _release = new internal_release<Fn_Ty>(fn);
        return *this;
    }

    void disable()
    {
        if (_release)
        {
            _release->disable();
            delete _release;
            _release = nullptr;
        }
    }

private:
    internal_release_base* _release;
};