#pragma once
#include <typeinfo.h>
#include <map>
#include <list>
#include <vector>
#include "util/singleton.h"

namespace Delegate
{

	enum ENUM_EVENT
	{
		//KEY = 0x00000000 ~ 0x0000FFFF
		KEYBOARD_PRESS = 0x00010000,
		KEYBOARD_REPEAT = 0x00020000,
		KEYBOARD_RELEASE = 0x00040000,
		KEYBOARD = KEYBOARD_PRESS | KEYBOARD_REPEAT | KEYBOARD_RELEASE,
		MOUSE_SCROLL = 0x00080000,
		MOUSE_MOUVE = 0x00080001,
		WINDOW_ZOOM = 0x00080002,
	};

	// IDelegate   提供接口的基类
	template<typename ReturnType, typename ...ParamType>
	class IDelegate
	{
	public:
		IDelegate() {}
		virtual ~IDelegate() {}
		virtual bool isType(const std::type_info& _type) = 0;
		virtual ReturnType invoke(ParamType ... params) = 0;
		virtual bool compare(IDelegate<ReturnType, ParamType...> *_delegate) const = 0;
	};


	//StaticDelegate 普通函数的委托
	template<typename ReturnType, typename ...ParamType>
	class CStaticDelegate :
		public IDelegate<ReturnType, ParamType...>
	{
	public:

		typedef  ReturnType(*Func)(ParamType...);

		CStaticDelegate(Func _func) : mFunc(_func) { }

		virtual bool isType(const std::type_info& _type) { return typeid(CStaticDelegate<ReturnType, ParamType...>) == _type; }

		virtual ReturnType invoke(ParamType ... params) { return mFunc(params...); }

		virtual bool compare(IDelegate<ReturnType, ParamType ...> *_delegate)const
		{
			if (0 == _delegate || !_delegate->isType(typeid(CStaticDelegate<ReturnType, ParamType ...>))) return false;
			CStaticDelegate<ReturnType, ParamType ...> * cast = static_cast<CStaticDelegate<ReturnType, ParamType ...>*>(_delegate);
			return cast->mFunc == mFunc;
		}

		virtual ~CStaticDelegate() {}
	private:
		Func mFunc;
	};

	//普通函数的委托特化版本
	template<typename ReturnType, typename ...ParamType>
	class CStaticDelegate<ReturnType(*)(ParamType ...)> :
		public IDelegate<ReturnType, ParamType ...>
	{
	public:

		//定义 Func 为 void (void) 函数类型指针。
		typedef  ReturnType(*Func)(ParamType...);

		CStaticDelegate(Func _func) : mFunc(_func) { }

		virtual bool isType(const std::type_info& _type) { return typeid(CStaticDelegate<ReturnType(*)(ParamType ...)>) == _type; }

		virtual ReturnType invoke(ParamType ... params) { return mFunc(params...); }

		virtual bool compare(IDelegate<ReturnType, ParamType ...> *_delegate)const
		{
			if (0 == _delegate || !_delegate->isType(typeid(CStaticDelegate<ReturnType(*)(ParamType ...)>))) return false;
			CStaticDelegate<ReturnType(*)(ParamType ...)> * cast = static_cast<CStaticDelegate<ReturnType(*)(ParamType ...)>*>(_delegate);
			return cast->mFunc == mFunc;
		}

		virtual ~CStaticDelegate() {}
	private:
		Func mFunc;
	};



	//成员函数委托
	template<typename T, typename ReturnType, typename ...ParamType>
	class CMethodDelegate :
		public IDelegate<ReturnType, ParamType...>
	{
	public:
		typedef ReturnType(T::*Method)(ParamType...);

		CMethodDelegate(T * _object, Method _method) : mObject(_object), mMethod(_method) { }

		virtual bool isType(const std::type_info& _type) { return typeid(CMethodDelegate<T, ReturnType, ParamType...>) == _type; }

		virtual ReturnType invoke(ParamType...params)
		{
			(mObject->*mMethod)(params...);
		}

		virtual bool compare(IDelegate<ReturnType, ParamType...> *_delegate) const
		{
			if (0 == _delegate || !_delegate->isType(typeid(CMethodDelegate<ReturnType, ParamType...>))) return false;
			CMethodDelegate<ReturnType, ParamType...>* cast = static_cast<CMethodDelegate<ReturnType, ParamType...>*>(_delegate);
			return cast->mObject == mObject && cast->mMethod == mMethod;
		}

		CMethodDelegate() {}
		virtual ~CMethodDelegate() {}
	private:
		T * mObject;
		Method mMethod;
	};

	//成员函数委托特化
	template<typename T, typename ReturnType, typename ...ParamType>
	class CMethodDelegate<T, ReturnType(T:: *)(ParamType...)> :
		public IDelegate<ReturnType, ParamType...>
	{
	public:
		typedef ReturnType(T::*Method)(ParamType...);

		CMethodDelegate(T * _object, Method _method) : mObject(_object), mMethod(_method) { }

		virtual bool isType(const std::type_info& _type) { return typeid(CMethodDelegate<T, ReturnType(T:: *)(ParamType...)>) == _type; }

		virtual ReturnType invoke(ParamType...params)
		{
			return (mObject->*mMethod)(params...);
		}

		virtual bool compare(IDelegate<ReturnType, ParamType...> *_delegate) const
		{
			if (0 == _delegate || !_delegate->isType(typeid(CMethodDelegate<T, ReturnType(T:: *)(ParamType...)>))) return false;
			CMethodDelegate<T, ReturnType(T:: *)(ParamType...)>* cast = static_cast<CMethodDelegate<T, ReturnType(T:: *)(ParamType...)>*>(_delegate);
			return cast->mObject == mObject && cast->mMethod == mMethod;
		}

		CMethodDelegate() {}
		virtual ~CMethodDelegate() {}
	private:
		T * mObject;
		Method mMethod;
	};



	//多播委托
	template<typename ReturnType, typename ...ParamType>
	class CMultiDelegate : public Singleton<CMultiDelegate<ReturnType, ParamType... >>
	{

	public:

		typedef std::list<IDelegate<ReturnType, ParamType...>*> ListDelegate;
		typedef typename ListDelegate::iterator ListDelegateIterator;
		typedef typename ListDelegate::const_iterator ConstListDelegateIterator;

		
		void Response(size_t event, ParamType... params)
		{
			auto it = mDirectory.find(event);
			if (it != mDirectory.end())
			{
				ListDelegateIterator iter = it->second.begin();
				while (iter != it->second.end())
				{
					(*iter)->invoke(params...);
					++iter;
				}
			}
				
		};


		void Register(size_t _event, IDelegate<ReturnType, ParamType...>* _delegate)
		{
			auto iter = mDirectory.find(_event);
			if (iter != mDirectory.end())
			{
				iter->second.push_back(_delegate);
			}
			else
			{
				mDirectory[_event] = ListDelegate(1, _delegate);
			}
		}


		bool Empty() const
		{
			return mDirectory.empty();
		}

		void Clear()
		{
			mDirectory.clear();
		}

		friend class Singleton<CMultiDelegate>;
	protected:
		CMultiDelegate() { }
		~CMultiDelegate() { mDirectory.clear(); }
	private:
		CMultiDelegate<ReturnType, ParamType...>(const CMultiDelegate& _event);
		CMultiDelegate<ReturnType, ParamType...>& operator=(const CMultiDelegate& _event);

	private:
		std::map<size_t, ListDelegate> mDirectory;
	};

	//多播委托返回值为void的特化版本
	template< typename ...ParamType>
	class CMultiDelegate<void, ParamType...>  : public Singleton<CMultiDelegate<void, ParamType...>>
	{

	public:
		typedef std::list<IDelegate<void, ParamType...>*> ListDelegate;
		typedef typename ListDelegate::iterator ListDelegateIterator;
		typedef typename ListDelegate::const_iterator ConstListDelegateIterator;


	public:
		void Response(size_t event, ParamType... params) 
		{
			auto it = mDirectory.find(event);
			if (it != mDirectory.end())
			{
				ListDelegateIterator iter = it->second.begin();
				while (iter != it->second.end())
				{
					(*iter)->invoke(params...);
					++iter;
				}
			}
		}

		void Register(size_t _event, IDelegate<void, ParamType...>* _delegate)
		{
			auto iter = mDirectory.find(_event);
			if (iter != mDirectory.end())
			{
				iter->second.push_back(_delegate);
			}
			else
			{
				mDirectory[_event] = ListDelegate(1, _delegate);
			}
		}

		bool Empty() const
		{
			return mDirectory.empty();
		}

		void Clear()
		{
			mDirectory.clear();
		}

		friend class Singleton<CMultiDelegate<void, ParamType...>>;
	protected:
		CMultiDelegate() { }
		~CMultiDelegate() { mDirectory.clear(); }

	private:
		CMultiDelegate<void, ParamType...>(const CMultiDelegate& _event);
		CMultiDelegate<void, ParamType...>& operator=(const CMultiDelegate& _event);

	private:
		std::map<size_t, ListDelegate> mDirectory;
	};


	template< typename T>
	CStaticDelegate<T>* newDelegate(T func)
	{
		return new CStaticDelegate<T>(func);
	}
	template< typename T, typename F>
	CMethodDelegate<T, F>* newDelegate(T * _object, F func)
	{
		return new CMethodDelegate<T, F>(_object, func);
	}
}


