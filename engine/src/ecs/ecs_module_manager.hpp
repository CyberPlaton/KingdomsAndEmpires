#pragma once
#include <config.hpp>
#include "ecs_module.hpp"

//------------------------------------------------------------------------------------------------------------------------
class irttr_object
{
public:
	irttr_object(stringview_t name) :
		m_name(name)
	{
	}

	stringview_t name() const{ return m_name.data(); }

protected:
	string_t m_name;
};


//------------------------------------------------------------------------------------------------------------------------
template<class TType>
class crttr_object : public irttr_object
{
public:
	crttr_object(stringview_t name) :
		irttr_object(name), m_class(name.data())
	{
	}

protected:
	rttr::registration::class_<TType> m_class;

protected:
	rttr::registration::class_<TType>& class_() { return m_class; }
};

//------------------------------------------------------------------------------------------------------------------------
template<class TType>
class cclass : public crttr_object<TType>
{
public:
	explicit cclass(stringview_t name) :
		crttr_object<TType>(name)
	{
		class_().constructor()(rttr::policy::ctor::as_object);
	}

	template<typename TPropType>
	cclass& prop(stringview_t name, TPropType TType::* field)
	{
		class_().property(name.data(), field);

		return *this;
	}

	template<typename TMethod>
	cclass& meth(stringview_t name, TMethod func)
	{
		class_().method(name.data(), std::move(func));

		return *this;
	}

};



#define ECS_MODULE(c) ecs::cmodule_manager::register_module(rttr::type::get<c>())

namespace ecs
{
	//- class responsible for loading and unloading modules for a world. Contains all current active modules.
	//-------------------------------------------------------------------------------------------------------------------------
	class ENGINE_API cmodule_manager
	{
	public:
		inline static vector_t<std::function<void()>> s_registered_funcs;
		inline static vector_t<irttr_object*> s_registered_objects;

		static void register_function(std::function<void()> func)
		{
			s_registered_funcs.push_back(std::move(func));
		}

		template<typename TCallable>
		static void register_rttr_object(TCallable&& func)
		{
			s_registered_objects.push_back(std::move(func()));
		}

		static void register_module(rttr::type type);
		static vector_t<stringview_t> registered_modules();

		cmodule_manager(flecs::world& w);
		~cmodule_manager();

		void init_modules();


	private:
		flecs::world& m_world;
		umap_t<stringview_t, flecs::entity> m_active_modules;
	};

} //- ecs