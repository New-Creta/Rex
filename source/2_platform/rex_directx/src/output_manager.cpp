#include "dxgi/core/dxgioutputmanager.h"
#include "dxgi/core/dxgioutput.h"
#include "dxgi/core/dxgiadaptermanager.h"
#include "dxgi/core/dxgiadapter.h"

//-------------------------------------------------------------------------
rex::dxgi::OutputManager::OutputManager(rex::dxgi::AdapterManager* manager, uint32 factoryVersion)
	: rex::OutputManager(manager)
	, m_factory_version(factoryVersion)
{
	load(manager);
}
//-------------------------------------------------------------------------
rex::dxgi::OutputManager::~OutputManager() = default;

//-------------------------------------------------------------------------
void rex::dxgi::OutputManager::load(rex::GpuManager * manager)
{
	rex::dxgi::Adapter* adapter = static_cast<rex::dxgi::Adapter*>(manager->getSelected());

	UINT i = 0;
	rex::dx::ComPtr<IDXGIOutput> output;
	while (adapter->get()->EnumOutputs(i, output.GetAddressOf()) != DXGI_ERROR_NOT_FOUND)
	{
		m_outputs.push_back(std::make_unique<rex::dxgi::Output>(std::move(output), adapter));
		++i;
	}
}

//-------------------------------------------------------------------------
rex::DisplayMode rex::dxgi::OutputManager::getHighestResolution(rex::FormatType format) const
{
	rex::DisplayMode display_mode = {};

	for (const auto& output : m_outputs)
	{
		auto display_modes = output->getDisplayModes(format);
		for (const rex::DisplayMode& mode : display_modes)
		{
			if (mode.width * mode.height > display_mode.width * display_mode.height)
			{
				display_mode = mode;
			}
		}
	}

	return display_mode;
}
//-------------------------------------------------------------------------
rex::DisplayMode rex::dxgi::OutputManager::getHighestRefreshRate(rex::FormatType format) const
{
	rex::DisplayMode display_mode = {};

	for (const auto& output : m_outputs)
	{
		auto display_modes = output->getDisplayModes(format);
		for (const rex::DisplayMode& mode : display_modes)
		{
			if (mode.refresh_rate > display_mode.refresh_rate)
			{
				display_mode = mode;
			}
		}
	}

	return display_mode;
}

//-------------------------------------------------------------------------
rex::DisplayMode rex::dxgi::OutputManager::getHighestResolution(rex::FormatType format, const rex::Gpu * gpu) const
{
	rex::DisplayMode display_mode = {};

	for (const auto& output : m_outputs)
	{
		if (output->getAdapter()->getDescription().device_id != gpu->getDescription().device_id)
			continue;

		auto display_modes = output->getDisplayModes(format);
		for (const rex::DisplayMode& mode : display_modes)
		{
			if (mode.width * mode.height > display_mode.width* display_mode.height)
			{
				display_mode = mode;
			}
		}
	}

	return display_mode;
}

//-------------------------------------------------------------------------
rex::DisplayMode rex::dxgi::OutputManager::getHighestRefreshRate(rex::FormatType format, const rex::Gpu * gpu) const
{
	rex::DisplayMode display_mode = {};

	for (const auto& output : m_outputs)
	{
		if (output->getAdapter()->getDescription().device_id != gpu->getDescription().device_id)
			continue;

		auto display_modes = output->getDisplayModes(format);
		for (const rex::DisplayMode& mode : display_modes)
		{
			if (mode.refresh_rate > display_mode.refresh_rate)
			{
				display_mode = mode;
			}
		}
	}

	return display_mode;
}

//-------------------------------------------------------------------------
rex::Output* rex::dxgi::OutputManager::getFirst() const
{
	return m_outputs.front().get();
}
//-------------------------------------------------------------------------
const rex::OutputVec& rex::dxgi::OutputManager::getAll() const
{
	return m_outputs;
}
