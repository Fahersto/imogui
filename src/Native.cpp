#include "Native.h"

#include "DrawHooks.h"
#include "Utility.h"
#include "Renderer.h"
#include "Detour.h"
#include "InputHandler.h"


#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_dx12.h"

#include "MidfunctionHook.h"

#include <Psapi.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")


namespace imogui
{
	namespace d3d12
	{
		hookftw::Detour nativePresentHook;
		hookftw::MidfunctionHook nativeExecuteCommandListsHook;

		ID3D12Device* d3d12Device = nullptr;
		ID3D12DescriptorHeap* d3d12DescriptorHeapBackBuffers = nullptr;
		ID3D12DescriptorHeap* d3d12DescriptorHeapImGuiRender = nullptr;
		ID3D12GraphicsCommandList* d3d12CommandList = nullptr;
		ID3D12CommandQueue* d3d12CommandQueue = nullptr;

		HMODULE mainModule;
		HWND mainWindow;

		// D3D12Core.dll+B5BE0 - 48 89 5C 24 08        - mov [rsp+08],rbx
		void hookExecuteCommandListsD3D12(ID3D12CommandQueue* queue, UINT NumCommandLists, ID3D12CommandList* ppCommandLists)
		{
			if (!d3d12CommandQueue)
			{
				d3d12CommandQueue = queue;
			}
		}

		struct FrameContext
		{
			ID3D12CommandAllocator* commandAllocator = nullptr;
			ID3D12Resource* main_render_target_resource = nullptr;
			D3D12_CPU_DESCRIPTOR_HANDLE main_render_target_descriptor;
		};

		UINT_PTR buffersCounts = -1;
		FrameContext* frameContext;

		//dxgi.dll+1FE0 - 48 89 5C 24 10        - mov [rsp+10],rbx
		void hookPresentD3D12(IDXGISwapChain3* pSwapChain, UINT syncInterval, UINT flags)
		{
			static bool initialised = false;

			if (!initialised)
			{
				if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D12Device), (void**)&d3d12Device)))
				{
					ImGui::CreateContext();

					unsigned char* pixels;
					int width, height;
					ImGuiIO& io = ImGui::GetIO(); (void)io;
					ImGui::StyleColorsDark();
					io.Fonts->AddFontDefault();
					io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
					io.IniFilename = NULL;

					DXGI_SWAP_CHAIN_DESC sdesc;
					pSwapChain->GetDesc(&sdesc);
					sdesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
					sdesc.OutputWindow = mainWindow;
					sdesc.Windowed = ((GetWindowLongPtr(mainWindow, GWL_STYLE) & WS_POPUP) != 0) ? false : true;

					buffersCounts = sdesc.BufferCount;
					frameContext = new FrameContext[buffersCounts];

					RECT rect;
					GetWindowRect(mainWindow, &rect);
					Renderer::Get().SetWidth(rect.right);
					Renderer::Get().SetHeight(rect.bottom);

					D3D12_DESCRIPTOR_HEAP_DESC descriptorImGuiRender = {};
					descriptorImGuiRender.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
					descriptorImGuiRender.NumDescriptors = buffersCounts;
					descriptorImGuiRender.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

					if (d3d12Device->CreateDescriptorHeap(&descriptorImGuiRender, IID_PPV_ARGS(&d3d12DescriptorHeapImGuiRender)) != S_OK)
					{
						return;
					}

					ID3D12CommandAllocator* allocator;
					if (d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)) != S_OK)
					{
						return;
					}

					for (size_t i = 0; i < buffersCounts; i++)
					{
						frameContext[i].commandAllocator = allocator;
					}

					if (d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator,
						NULL, IID_PPV_ARGS(&d3d12CommandList)) != S_OK || d3d12CommandList->Close() != S_OK)
					{
						return;
					}

					D3D12_DESCRIPTOR_HEAP_DESC descriptorBackBuffers;
					descriptorBackBuffers.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
					descriptorBackBuffers.NumDescriptors = buffersCounts;
					descriptorBackBuffers.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
					descriptorBackBuffers.NodeMask = 1;

					if (d3d12Device->CreateDescriptorHeap(&descriptorBackBuffers, IID_PPV_ARGS(&d3d12DescriptorHeapBackBuffers)) != S_OK)
					{
						return;
					}

					const auto rtvDescriptorSize = d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
					D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d12DescriptorHeapBackBuffers->GetCPUDescriptorHandleForHeapStart();

					for (size_t i = 0; i < buffersCounts; i++) {
						ID3D12Resource* pBackBuffer = nullptr;

						frameContext[i].main_render_target_descriptor = rtvHandle;
						pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
						d3d12Device->CreateRenderTargetView(pBackBuffer, nullptr, rtvHandle);
						frameContext[i].main_render_target_resource = pBackBuffer;
						rtvHandle.ptr += rtvDescriptorSize;
					}

					ImGui_ImplWin32_Init(mainWindow);
					ImGui_ImplDX12_Init(d3d12Device, buffersCounts,
						DXGI_FORMAT_R8G8B8A8_UNORM, d3d12DescriptorHeapImGuiRender,
						d3d12DescriptorHeapImGuiRender->GetCPUDescriptorHandleForHeapStart(),
						d3d12DescriptorHeapImGuiRender->GetGPUDescriptorHandleForHeapStart());

					ImGui_ImplDX12_CreateDeviceObjects();

					InputHandler::HookWndProc(mainWindow);
				}
				initialised = true;
			}

			if (!d3d12CommandQueue)
			{
				return;
			}

			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			Renderer::Get().BeginScene();
			DrawHooks::renderCallback(Renderer::Get());
			Renderer::Get().EndScene();

			FrameContext& currentFrameContext = frameContext[pSwapChain->GetCurrentBackBufferIndex()];
			currentFrameContext.commandAllocator->Reset();

			D3D12_RESOURCE_BARRIER barrier;
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = currentFrameContext.main_render_target_resource;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

			d3d12CommandList->Reset(currentFrameContext.commandAllocator, nullptr);
			d3d12CommandList->ResourceBarrier(1, &barrier);
			d3d12CommandList->OMSetRenderTargets(1, &currentFrameContext.main_render_target_descriptor, FALSE, nullptr);
			d3d12CommandList->SetDescriptorHeaps(1, &d3d12DescriptorHeapImGuiRender);

			ImGui::Render();
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), d3d12CommandList);

			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

			d3d12CommandList->ResourceBarrier(1, &barrier);
			d3d12CommandList->Close();

			d3d12CommandQueue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(&d3d12CommandList));

			return;
		}
	}

	void Native::Hook(Renderapi api, std::function<void(Renderer&)> drawCallback)
	{
		DrawHooks::renderCallback = drawCallback;
		int8_t* hookAddress = nullptr;

#ifdef _WIN64
		switch (api)
		{
		case Renderapi::OPENGL:
			assert(false);
			break;
		case Renderapi::DIRECTX9:
			assert(false);
			break;
		case Renderapi::DIRECTX11:
			assert(false);
			break;
		case Renderapi::DIRECTX12:
			bool WindowFocus = false;
			while (WindowFocus == false)
			{
				DWORD ForegroundWindowProcessID;
				GetWindowThreadProcessId(GetForegroundWindow(), &ForegroundWindowProcessID);
				if (GetCurrentProcessId() == ForegroundWindowProcessID)
				{
					d3d12::mainWindow = GetForegroundWindow();
					WindowFocus = true;
				}
			}

			static hookftw::MidfunctionHook presentHook;
			static hookftw::MidfunctionHook executeCommandListHook;

			// D3D12Core.dll+B5BE0 - 48 89 5C 24 08        - mov [rsp+08],rbx
			executeCommandListHook.Hook(
				(int8_t*)GetModuleHandleA("D3D12Core.dll") + 0xB5BE0,
				[](hookftw::context* ctx)
				{
					d3d12::hookExecuteCommandListsD3D12(
						(ID3D12CommandQueue*)ctx->rcx,
						ctx->rdx,
						(ID3D12CommandList*)ctx->r8
					);
				}
			);

			//dxgi.dll+1FE0 - 48 89 5C 24 10        - mov [rsp+10],rbx
			presentHook.Hook(
				Utility::Scan("dxgi.dll", "48 89 74 24 20 55 57 41 56 48 8D 6C 24 90"),
				//	(int8_t*)GetModuleHandleA("dxgi.dll") + 0x1FE5,
				[](hookftw::context* ctx)
				{
					d3d12::hookPresentD3D12(
						(IDXGISwapChain3*)ctx->rcx,
						ctx->rdx,
						ctx->r8
					);
				}
			);
			break;
		}
#elif _WIN32
		switch (api)
		{
		case Renderapi::OPENGL:
			assert(false);
			break;
		case Renderapi::DIRECTX9:
			assert(false);
			break;
		case Renderapi::DIRECTX11:
			assert(false);
			break;
		}
#endif


	}

	void Native::Unhook()
	{
		d3d12::nativeExecuteCommandListsHook.Unhook();
		d3d12::nativePresentHook.Unhook();
		InputHandler::UnhookWndProc();
	}

}