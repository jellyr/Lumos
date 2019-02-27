#include "LM.h"
#include "VKDevice.h"
#include "VKVertexBuffer.h"

namespace Lumos
{ 
	namespace graphics
	{
		static uint BufferUsageToOpenVK(const BufferUsage usage)
		{
			switch (usage)
			{
			case BufferUsage::STATIC:  return 0;
			case BufferUsage::DYNAMIC: return 0;
			case BufferUsage::STREAM:  return 0;
			}
			return 0;
		}

		VKVertexBuffer::VKVertexBuffer(BufferUsage usage)
			: VKBuffer(), m_Usage(usage), m_Size(0)
		{
		}

		VKVertexBuffer::~VKVertexBuffer()
		{
		}

		void VKVertexBuffer::Resize(uint size)
		{
			m_Size = size;

		}

		void VKVertexBuffer::SetLayout(const graphics::BufferLayout& bufferLayout)
		{
			m_Layout = bufferLayout;
		}

		void VKVertexBuffer::SetData(uint size, const void* data)
		{
			VKBuffer::Init(vk::BufferUsageFlagBits::eVertexBuffer, size, data);
		}


		void VKVertexBuffer::SetDataSub(uint size, const void* data, uint offset)
		{
			VKBuffer::Init(vk::BufferUsageFlagBits::eVertexBuffer, size, data);
		}


		void* VKVertexBuffer::GetPointerInternal()
		{
			void* temp;
			VKDevice::Instance()->GetDevice().mapMemory(m_Memory, 0, m_Size, vk::MemoryMapFlagBits(), &temp);
			return temp;
		}

		void VKVertexBuffer::ReleasePointer()
		{
			VKDevice::Instance()->GetDevice().unmapMemory(m_Memory);
		}

		void VKVertexBuffer::Bind()
		{
		}

		void VKVertexBuffer::Unbind()
		{
		}
	}
}