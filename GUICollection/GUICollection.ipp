#pragma once
#include "GUICollection.h"

namespace gui {
	namespace internal {

		template<auto StartFunc, void (*EndFunc)(), bool AlwaysCallEnd, bool StartIsOptional>
		template<typename ...Args>
		BeginEndBound<StartFunc, EndFunc, AlwaysCallEnd, StartIsOptional>::BeginEndBound(Args... args)
			: isOpen{ CallStartFuncBasedOnParams(args...) }
		{
		}

		template<auto StartFunc, void(*EndFunc)(), bool AlwaysCallEnd, bool StartIsOptional>
		template<typename ...Args>
		bool BeginEndBound<StartFunc, EndFunc, AlwaysCallEnd, StartIsOptional>::CallStartFunc(Args... args)
		{
			if constexpr (std::is_same_v<util::ReturnType_t<StartFunc>, bool>)
				return StartFunc(args...);
			else
			{
				StartFunc(args...);
				return true;
			}
		}

		template<auto StartFunc, void(*EndFunc)(), bool AlwaysCallEnd, bool StartIsOptional>
		template<typename ...Args>
		bool BeginEndBound<StartFunc, EndFunc, AlwaysCallEnd, StartIsOptional>::CallStartFuncBasedOnParams(Args... args)
		{
			if constexpr (StartIsOptional)
				return [](bool first, auto&... args) -> bool {
					if (!first)
						return false;
					return CallStartFunc(args...);
				}(args...);
			else
				return CallStartFunc(args...);
		}

		template<auto StartFunc, void (*EndFunc)(), bool AlwaysCallEnd, bool StartIsOptional>
		BeginEndBound<StartFunc, EndFunc, AlwaysCallEnd, StartIsOptional>::~BeginEndBound()
		{
			if constexpr (static_cast<bool>(EndFunc))
				if (isOpen || AlwaysCallEnd)
					EndFunc();
		}

		template<auto StartFunc, void (*EndFunc)(), bool AlwaysCallEnd, bool StartIsOptional>
		BeginEndBound<StartFunc, EndFunc, AlwaysCallEnd, StartIsOptional>::operator bool() const
		{
			return isOpen;
		}

		template<auto StartFunc, void(*EndFunc)(), bool AlwaysCallEnd, bool StartIsOptional>
		bool BeginEndBound<StartFunc, EndFunc, AlwaysCallEnd, StartIsOptional>::GetIsOpen()
		{
			return isOpen;
		}

		template<typename ...Args>
		std::string format(TextTypeExtra text, const Args& ...args)
		{
			return std::vformat(text, std::make_format_args(args...));
		}

	}

	template<typename ...Args>
	Tooltip::Tooltip([[maybe_unused]] internal::TextTypeForFormatting fmt, [[maybe_unused]] const Args&... args)
#ifndef IMGUI_DISABLE
		: internal::BeginEndBound_Tooltip{ false }
#endif
	{
#ifndef IMGUI_DISABLE
#ifdef GUI_FEATURE_CPP20_FORMAT
		ImGui::SetTooltip("%s", internal::format(fmt, args...).c_str());
#else // GUI_FEATURE_CPP20_FORMAT / !GUI_FEATURE_CPP20_FORMAT
		ImGui::SetTooltip(fmt, args...);
#endif // !GUI_FEATURE_CPP20_FORMAT
#endif // !IMGUI_DISABLE
	}

	template<typename ...Args>
	void Text([[maybe_unused]] internal::TextTypeForFormatting fmt, [[maybe_unused]] const Args&... args)
	{
#ifndef IMGUI_DISABLE
		if constexpr (sizeof...(args) == 0)
			ImGui::TextUnformatted(fmt);
		else
#ifdef GUI_FEATURE_CPP20_FORMAT
			ImGui::TextUnformatted(internal::format(fmt, args...).c_str());
#else // GUI_FEATURE_CPP20_FORMAT / !GUI_FEATURE_CPP20_FORMAT
			ImGui::Text(fmt, args...);
#endif // !GUI_FEATURE_CPP20_FORMAT
#endif // !IMGUI_DISABLE
	}

	template <typename... Args>
	void TextCentered([[maybe_unused]] internal::TextTypeForFormatting fmt, const Args&... args)
	{
#ifndef IMGUI_DISABLE
		auto CenterCursorPos{ [](const internal::TextTypeForFormatting& text) -> void {
			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(text, text.data() + text.size()).x) * 0.5f);
		} };

		if constexpr (sizeof...(args) == 0)
		{
			CenterCursorPos(fmt);
			Text(fmt);
		}
		else
		{
#ifdef GUI_FEATURE_CPP20_FORMAT
			std::string printedText{ internal::format(fmt, args...) };
#else // GUI_FEATURE_CPP20_FORMAT / !GUI_FEATURE_CPP20_FORMAT
			std::string printedText{ util::sprintf(fmt, args...) };
#endif // !GUI_FEATURE_CPP20_FORMAT
			CenterCursorPos(printedText);
			Text(printedText);
		}
#endif // !IMGUI_DISABLE
	}

	template<typename ...Args>
	void TextColored([[maybe_unused]] const Vec4& color, [[maybe_unused]] internal::TextTypeForFormatting fmt, [[maybe_unused]] const Args&... args)
	{
#ifndef IMGUI_DISABLE
		if constexpr (sizeof...(args) == 0)
			ImGui::TextColored(color, "%s", fmt.data());
		else
#ifdef GUI_FEATURE_CPP20_FORMAT
			ImGui::TextColored(color, "%s", internal::format(fmt, args...).c_str());
#else // GUI_FEATURE_CPP20_FORMAT / !GUI_FEATURE_CPP20_FORMAT
			ImGui::TextColored(color, fmt, args...);
#endif // !GUI_FEATURE_CPP20_FORMAT
#endif // !IMGUI_DISABLE
	}

	template<typename ...Args>
	void TextWrapped([[maybe_unused]] internal::TextTypeForFormatting fmt, [[maybe_unused]] const Args&... args)
	{
#ifndef IMGUI_DISABLE
		if constexpr (sizeof...(args) == 0)
			ImGui::TextWrapped("%s", fmt.data());
		else
#ifdef GUI_FEATURE_CPP20_FORMAT
			ImGui::TextWrapped("%s", internal::format(fmt, args...).c_str());
#else // GUI_FEATURE_CPP20_FORMAT / !GUI_FEATURE_CPP20_FORMAT
			ImGui::TextWrapped(fmt, args...);
#endif // !GUI_FEATURE_CPP20_FORMAT
#endif // !IMGUI_DISABLE
	}

	template<typename ...Args>
	void TextDisabled([[maybe_unused]] internal::TextTypeForFormatting fmt, [[maybe_unused]] const Args&... args)
	{
#ifndef IMGUI_DISABLE
		if constexpr (sizeof...(args) == 0)
			ImGui::TextDisabled("%s", fmt.data());
		else
#ifdef GUI_FEATURE_CPP20_FORMAT
			ImGui::TextDisabled("%s", internal::format(fmt, args...).c_str());
#else // GUI_FEATURE_CPP20_FORMAT / !GUI_FEATURE_CPP20_FORMAT
			ImGui::TextDisabled(fmt, args...);
#endif // !GUI_FEATURE_CPP20_FORMAT
#endif // !IMGUI_DISABLE
	}

	template<typename T, typename ElemDrawFuncType>
	bool VarContainer([[maybe_unused]] internal::TextTypeForFormatting label, [[maybe_unused]] std::vector<T>* v, [[maybe_unused]] ElemDrawFuncType elemDrawFunc)
	{
#ifndef IMGUI_DISABLE
		bool modified{ false };

		Text(label);
		SameLine();

		int contSize{ static_cast<int>(v->size()) };
		if (VarInput("Size", &contSize))
		{
			v->resize(static_cast<size_t>(std::max(contSize, 0)));
			modified = true;
		}

		Indent indentation{};
		for (size_t i{}, end{ v->size() }; i < end; ++i)
		{
			SetID id{ static_cast<int>(i) };
#ifdef GUI_FEATURE_CPP20_FORMAT
			Text("{}", i);
#else
			Text("%d", i);
#endif
			SameLine();
			if constexpr (std::is_same_v<decltype(elemDrawFunc(v->at(i))), void>)
				elemDrawFunc(v->at(i));
			else
				if (elemDrawFunc(v->at(i)))
					modified = true;
		}

		return modified;
#else
		return false;
#endif
	}

	template<typename DataType>
	PayloadSource<DataType>::PayloadSource([[maybe_unused]] internal::TextType identifier, [[maybe_unused]] const DataType& data, [[maybe_unused]] FLAG_PAYLOAD_SOURCE flags)
#ifndef IMGUI_DISABLE
		: internal::BeginEndBound_PayloadSource{ +flags }
#endif
	{
#ifndef IMGUI_DISABLE
		if (GetIsOpen())
			SetPayloadTarget(identifier, data);
#endif
	}

	template<typename DataType>
	void PayloadSource<DataType>::SetPayloadTarget([[maybe_unused]] internal::TextType identifier, [[maybe_unused]] const DataType& data)
	{
#ifndef IMGUI_DISABLE
		ImGui::SetDragDropPayload(identifier, &data, sizeof(DataType));
#endif
	}

	namespace internal {
		// These classes exist to work around the limitation of no partial specializations allowed for functions by utilizing
		// the allowance of partial specializations of classes.
		template <typename DataType, typename FunctionType>
		struct PayloadTargetClass
		{
			static void Invoke([[maybe_unused]] internal::TextType identifier, [[maybe_unused]] FunctionType onReceive, [[maybe_unused]] FLAG_PAYLOAD_TARGET flags)
			{
#ifndef IMGUI_DISABLE
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload{ ImGui::AcceptDragDropPayload(identifier, +flags) })
						onReceive(*reinterpret_cast<const DataType*>(payload->Data));

					ImGui::EndDragDropTarget();
				}
#endif
			}
		};
		template <typename FunctionType>
		struct PayloadTargetClass<std::string, FunctionType>
		{
			static void Invoke([[maybe_unused]] internal::TextType identifier, [[maybe_unused]] FunctionType onReceive, [[maybe_unused]] FLAG_PAYLOAD_TARGET flags)
			{
#ifndef IMGUI_DISABLE
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload{ ImGui::AcceptDragDropPayload(identifier, +flags) })
						onReceive(reinterpret_cast<const char*>(payload->Data));

					ImGui::EndDragDropTarget();
				}
#endif
			}
		};
	}
	template<typename DataType, typename FunctionType>
		requires std::invocable<FunctionType, const DataType&>
	void PayloadTarget([[maybe_unused]] internal::TextType identifier, [[maybe_unused]] FunctionType onReceive, [[maybe_unused]] FLAG_PAYLOAD_TARGET flags)
	{
#ifndef IMGUI_DISABLE
		internal::PayloadTargetClass<DataType, FunctionType>::Invoke(identifier, onReceive, flags);
#endif
	}

	template<typename DataType, typename FunctionType>
	void PayloadTargetRect([[maybe_unused]] internal::TextType payloadIdentifier, [[maybe_unused]] Vec2 size, [[maybe_unused]] internal::TextTypeExtra displayText, [[maybe_unused]] FunctionType onReceive, [[maybe_unused]] FLAG_PAYLOAD_TARGET flags)
	{
#ifndef IMGUI_DISABLE
		Vec2 topLeft{ GetScreenCursorPos() };

		DrawRect(topLeft, topLeft + size, GetStyleColor(gui::FLAG_STYLE_COLOR::FRAME_BG), 5.0f);
		InvisibleButton("##PayloadTarget", size);
		PayloadTarget<DataType>(payloadIdentifier, onReceive, flags);

		if (!displayText.empty())
		{
			Vec2 textSize{ CalcTextSize(displayText) };
			DrawText(displayText, topLeft + (size - textSize) * 0.5f, GetStyleColor(gui::FLAG_STYLE_COLOR::TEXT));
		}
#endif
	}

	template<typename ContType>
		requires util::ConvertibleToCArray<ContType>&& std::is_same_v<typename ContType::value_type, const char*>
	Combo::Combo([[maybe_unused]] internal::TextType label, [[maybe_unused]] const ContType& data, [[maybe_unused]] internal::TextType selectedValue, [[maybe_unused]] int* outSelectedIndex)
		: shouldCallEndCombo{ false }
	{
#ifndef IMGUI_DISABLE
		// Find the index of the selected value.
		for (int i{}; static_cast<size_t>(i) < std::size(data); ++i)
			if (std::strcmp(data[i], selectedValue) == 0)
			{
				*outSelectedIndex = i;
				break;
			}
		// Draw the combo
		CallCombo(label, std::data(data), std::size(data), outSelectedIndex);
#endif
	}

	template<typename ContType>
		requires util::ConvertibleToCArray<ContType>&& std::is_same_v<typename ContType::value_type, const char*>
	Combo::Combo([[maybe_unused]] internal::TextType label, [[maybe_unused]] const ContType& data, [[maybe_unused]] int* selectedIndex)
		: shouldCallEndCombo{ false }
	{
#ifndef IMGUI_DISABLE
		CallCombo(label, std::data(data), std::size(data), selectedIndex);
#endif
	}

	template<typename ContType>
		requires util::ConvertibleToCArray<ContType>&& std::is_same_v<typename ContType::value_type, float>
	void PlotLines([[maybe_unused]] internal::TextType label, [[maybe_unused]] const ContType& cont, [[maybe_unused]] Vec2 graphSize, [[maybe_unused]] float scaleMin, [[maybe_unused]] float scaleMax, [[maybe_unused]] const char* overlayText)
	{
#ifndef IMGUI_DISABLE
		ImGui::PlotLines(label, std::data(cont), static_cast<int>(std::size(cont)), 0, overlayText, scaleMin, scaleMax, graphSize);
#endif
	}

	template<typename NormalFuncType, typename RenameCallbackFuncType>
		requires std::regular_invocable<NormalFuncType>&& std::regular_invocable<RenameCallbackFuncType, const char*>
	void Renamable::Wrap([[maybe_unused]] internal::TextType currName, [[maybe_unused]] NormalFuncType normalRoute, [[maybe_unused]] RenameCallbackFuncType onRenameRoute)
	{
#ifndef IMGUI_DISABLE
		int currId{ GetCurrID() };
		if (idOfItemBeingRenamed != currId)
		{
			// Draw the user's normal draw routine
			normalRoute();

			// Add on a right click context menu
			if (ItemContextMenu contextMenu{ "GenericRenameContextMenu" })
				if (MenuItem("Rename"))
				{
					idOfItemBeingRenamed = currId;
					stealFocus = true;
					STRNCPY(buffer, currName, 256);
				}
		}
		else
		{
			// Draw an input text box for renaming
			SetID id{ "GenericRename" };
			SetNextItemWidth(-1.0f);
			if (stealFocus)
				SetKeyboardFocusHere();

			if (TextBox("", buffer, 256, FLAG_INPUT_TEXT::ENTER_RETURNS_TRUE))
			{
				onRenameRoute(buffer);
				idOfItemBeingRenamed = -1;
			}

			// Check for focus loss or escape key pressed
			if (stealFocus)
			{
				// SetKeyboardFocusHere() is not immediate, so we need to explicitly make ImGui update to the new focus before we test for IsItemFocused()
				ImGui::NavMoveRequestApplyResult();
				stealFocus = false;
			}
			if (!IsItemFocused() || IsKeyPressed(KEY::ESC))
				idOfItemBeingRenamed = -1;
		}
#endif
	}

}