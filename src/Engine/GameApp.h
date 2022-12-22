#pragma once

#include "gKit/app.h"

namespace FalconEye
{

	class GameApp : public App
	{

	public:
		virtual int init() override
		{
			throw std::logic_error("The method or operation is not implemented.");
		}


		virtual int quit() override
		{
			throw std::logic_error("The method or operation is not implemented.");
		}


		virtual int update(const float time, const float delta) override
		{
			throw std::logic_error("The method or operation is not implemented.");
		}


		virtual int render() override
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

	private:

	};

} // end namespace FalconEye