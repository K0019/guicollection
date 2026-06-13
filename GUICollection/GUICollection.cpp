#include "GUICollection.h"

namespace gui {

	char Renamable::buffer[256]{};
	int Renamable::idOfItemBeingRenamed{ -1 };
	bool Renamable::stealFocus{ false };


	namespace internal {

#ifndef IMGUI_DISABLE
		bool BeginChild(const char* str_id, const ImVec2& size, ImGuiChildFlags child_flags, ImGuiWindowFlags window_flags)
		{
			return ImGui::BeginChild(str_id, size, child_flags, window_flags);
		}
		bool TreeNodeEx(const char* label, ImGuiTreeNodeFlags flags)
		{
			return ImGui::TreeNodeEx(label, flags);
		}
		bool CollapsingHeader(const char* label, ImGuiTreeNodeFlags flags)
		{
			return ImGui::CollapsingHeader(label, flags);
		}
		void PushStyleVar(ImGuiStyleVar idx, const ImVec2& val)
		{
			ImGui::PushStyleVar(idx, val);
		}
		void PopStyleVar()
		{
			ImGui::PopStyleVar();
		}
		void PushStyleColor(ImGuiCol idx, const ImVec4& val)
		{
			ImGui::PushStyleColor(idx, val);
		}
		void PopStyleColor()
		{
			ImGui::PopStyleColor();
		}
#endif

		ContainerBase::ContainerBase(TextTypeExtra title, const Vec2& initialDimensions, FLAG_COND windowSizeCondFlags)
			: isOpen{ true }
			, title{ title }
			, initialDimensions{ initialDimensions }
			, windowSizeCondFlags{ windowSizeCondFlags }
		{
		}

		void ContainerBase::Draw([[maybe_unused]] int id)
		{
#ifndef IMGUI_DISABLE
			if (!isOpen)
				return;

			ImGui::SetNextWindowSize(initialDimensions, +windowSizeCondFlags);
			DrawContainer(id);
#endif
		}

		TextType::TextType(const char* text)
			: text{ text }
		{
		}
		TextType::TextType(const std::string_view& text)
			: text{ text.data() }
		{
		}
		TextType::TextType(const std::string& text)
			: text{ text.c_str() }
		{
		}

		TextType::operator const char* () const
		{
			return text;
		}
		const char* TextType::data() const
		{
			return text;
		}
		size_t TextType::size() const
		{
			return std::strlen(text);
		}
		bool TextType::empty() const
		{
			return *text == '\0';
		}

		TextTypeExtra::TextTypeExtra(const char* text)
			: std::string_view{ text }
		{
		}
		TextTypeExtra::TextTypeExtra(const std::string_view& text)
			: std::string_view{ text }
		{
		}
		TextTypeExtra::TextTypeExtra(const std::string& text)
			: std::string_view{ text }
		{
		}

		TextTypeExtra::operator const char* () const
		{
			return data();
		}

	}

	Window::Window(internal::TextTypeExtra title, const Vec2& initDimensions, FLAG_WINDOW windowFlags)
		: internal::ContainerBase{ title, initDimensions, FLAG_COND::FIRST_USE_EVER }
		, windowFlags{ windowFlags }
	{
	}

	void Window::DrawContainer([[maybe_unused]] int id)
	{
#ifndef IMGUI_DISABLE
		if (ImGui::Begin((id <= 0 ? title.c_str() : (title + "##" + std::to_string(id)).c_str()), &isOpen, +windowFlags))
		{
			DrawContents();

			// Check if the user closed the window this frame.
			if (!isOpen)
				OnOpenStateChanged();
		}
		ImGui::End();
#endif
	}

	bool Window::GetIsOpen() const
	{
		return isOpen;
	}

	void Window::SetIsOpen(bool newIsOpen)
	{
		if (isOpen != newIsOpen)
		{
			isOpen = newIsOpen;
			OnOpenStateChanged();
		}
	}

	void Window::OnOpenStateChanged()
	{
	}

	Child::Child([[maybe_unused]] internal::TextType str_id, [[maybe_unused]] const Vec2& size, [[maybe_unused]] FLAG_CHILD child_flags, [[maybe_unused]] FLAG_WINDOW window_flags)
#ifndef IMGUI_DISABLE
		: internal::BeginEndBound_Child{ str_id, size, +child_flags, +window_flags }
#endif
	{
	}

	Popup::Popup([[maybe_unused]] internal::TextType str_id, [[maybe_unused]] FLAG_WINDOW flags)
#ifndef IMGUI_DISABLE
		: internal::BeginEndBound_Popup{ str_id, +flags }
#endif
	{
	}

	void Popup::Open([[maybe_unused]] internal::TextType identifier)
	{
#ifndef IMGUI_DISABLE
		ImGui::OpenPopup(identifier);
#endif
	}

	void Popup::Close()
	{
#ifndef IMGUI_DISABLE
		ImGui::CloseCurrentPopup();
#endif
	}

	bool Popup::WasOpenedThisFrame()
	{
#ifndef IMGUI_DISABLE
		return ImGui::IsWindowAppearing();
#else
		return false;
#endif
	}

	PopupWindow::PopupWindow(internal::TextTypeExtra title, const Vec2& initDimensions)
		: internal::ContainerBase{ title, initDimensions, FLAG_COND::APPEARING }
		, flags{}
	{
	}

	void PopupWindow::DrawContainer([[maybe_unused]] int id)
	{
#ifndef IMGUI_DISABLE
		if (id <= 0)
			DrawContainer_Popup(title);
		else
			DrawContainer_Popup(title + "##" + std::to_string(id));
#endif
	}

	void PopupWindow::DrawContainer_Popup([[maybe_unused]] internal::TextType effectiveTitle)
	{
#ifndef IMGUI_DISABLE
		ImGui::OpenPopup(effectiveTitle);

		bool* isOpenPtr{ +(flags & FLAG::NO_CLOSE_BUTTON) ? nullptr : &isOpen };
		if (ImGui::BeginPopupModal(effectiveTitle, isOpenPtr))
		{
			DrawContents();
			ImGui::EndPopup();
		}
#endif
	}

	Tooltip::Tooltip()
#ifndef IMGUI_DISABLE
		: internal::BeginEndBound_Tooltip{ true }
#endif
	{
	}

	SetID::SetID([[maybe_unused]] int id)
	{
#ifndef IMGUI_DISABLE
		ImGui::PushID(id);
#endif
	}
	SetID::SetID([[maybe_unused]] size_t id)
	{
#ifndef IMGUI_DISABLE
		ImGui::PushID(static_cast<int>(id));
#endif
	}
	SetID::SetID([[maybe_unused]] internal::TextType label)
	{
#ifndef IMGUI_DISABLE
		ImGui::PushID(label);
#endif
	}

	SetID::~SetID()
	{
#ifndef IMGUI_DISABLE
		ImGui::PopID();
#endif
	}

	int GetCurrID()
	{
#ifndef IMGUI_DISABLE
		return GImGui->CurrentWindow->IDStack.back();
#else
		return 0;
#endif
	}

	float GetFrameHeightWithSpacing()
	{
#ifndef IMGUI_DISABLE
		return ImGui::GetFrameHeightWithSpacing();
#else
		return 1.0f;
#endif
	}

	bool IsCurrWindowFocused()
	{
#ifndef IMGUI_DISABLE
		return ImGui::GetCurrentContext()->WindowsFocusOrder.Size == ImGui::GetCurrentContext()->CurrentWindow->FocusOrder + 1;
#else
		return true;
#endif
	}

	void SetItemDefaultFocus()
	{
#ifndef IMGUI_DISABLE
		ImGui::SetItemDefaultFocus();
#endif
	}

	bool IsItemFocused()
	{
#ifndef IMGUI_DISABLE
		return ImGui::IsItemFocused();
#else
		return false;
#endif
	}

	bool IsItemHovered()
	{
#ifndef IMGUI_DISABLE
		return ImGui::IsItemHovered();
#else
		return false;
#endif
	}

	float GetFontSize()
	{
#ifndef IMGUI_DISABLE
		return ImGui::GetFontSize();
#else
		return 1.0f;
#endif
	}

	Vec2 CalcTextSize(internal::TextTypeExtra text)
	{
#ifndef IMGUI_DISABLE
		return ImGui::CalcTextSize(text, text.data() + text.size());
#else
		return Vec2{};
#endif
	}

	void AlignTextToFramePadding()
	{
#ifndef IMGUI_DISABLE
		ImGui::AlignTextToFramePadding();
#endif
	}

	bool TextBox([[maybe_unused]] const char* label, [[maybe_unused]] char* text, [[maybe_unused]] size_t maxSize, [[maybe_unused]] FLAG_INPUT_TEXT flags)
	{
#ifndef IMGUI_DISABLE
		return ImGui::InputText(label, text, maxSize, +flags);
#else
		return false;
#endif
	}
	bool TextBox([[maybe_unused]] const char* label, [[maybe_unused]] std::string* text, [[maybe_unused]] FLAG_INPUT_TEXT flags)
	{
#ifndef IMGUI_DISABLE
		return ImGui::InputText(label, text, +flags);
#else
		return false;
#endif
	}

	void TextBoxReadOnly([[maybe_unused]] const char* label, [[maybe_unused]] const char* text, [[maybe_unused]] size_t size)
	{
#ifndef IMGUI_DISABLE
		gui::SetStyleColor frameColor{ gui::FLAG_STYLE_COLOR::FRAME_BG, gui::GetStyleColor(gui::FLAG_STYLE_COLOR::FRAME_BG, 0.65f) };
		ImGui::InputText(label, const_cast<char*>(text), size, +FLAG_INPUT_TEXT::READ_ONLY);
#endif
	}
	void TextBoxReadOnly(const char* label, internal::TextTypeExtra text)
	{
		TextBoxReadOnly(label, text.data(), text.size() + 1);
	}

	bool PassFilterSimple(internal::TextTypeExtra filterText, internal::TextTypeExtra candidateText)
	{
		if (filterText.empty())
			return true;

		// Candidates cannot pass the filter if it is shorter than the filter
		if (candidateText.size() < filterText.size())
			return false;

		// This checks up to the number of characters within the filter
		return std::equal(filterText.begin(), filterText.end(), candidateText.begin(), candidateText.begin() + filterText.size(),
			[](char a, char b) -> bool { return std::tolower(a) == std::tolower(b); });
	}

	AdvancedFilter::AdvancedFilter(internal::TextTypeExtra filterText)
	{
		// Split filter by ','
		for (auto&& termRangeRaw : filterText | std::views::split(','))
		{
			// Trim whitespace
			auto termRange{
				termRangeRaw
				| std::views::drop_while([](char c) -> bool { return std::isspace(c); }) // Trim left
				| std::views::reverse
				| std::views::drop_while([](char c) -> bool { return std::isspace(c); }) // Trim right
				| std::views::reverse
			};

			if (termRange.empty())
				continue;

			// Sort into positive or negative filter terms
			std::string_view termView{ &*termRange.begin(), static_cast<size_t>(std::ranges::distance(termRange)) };
			if (termView[0] == '-')
				negativeFilter.emplace_back(termView.substr(1));
			else
				positiveFilter.emplace_back(std::move(termView));
		}
	}

	bool AdvancedFilter::PassFilter(internal::TextTypeExtra candidateText)
	{
		// Case insensitive predicate
		auto pred{ [](char a, char b) -> bool { return std::tolower(a) == std::tolower(b); } };

		// Fail if any negative terms are found
		for (const std::string_view& filterTerm : negativeFilter)
			if (std::ranges::search(candidateText, filterTerm, pred))
				return false;

		// Pass if no positive terms are specified
		if (positiveFilter.empty())
			return true;

		// Pass if any positive terms are found
		for (const std::string_view& filterTerm : positiveFilter)
			if (std::ranges::search(candidateText, filterTerm, pred))
				return true;

		// Fail if no positive terms are found
		return false;
	}

	SetTextWrapPos::SetTextWrapPos([[maybe_unused]] float wrap_local_pos_x)
#ifndef IMGUI_DISABLE
		: internal::BeginEndBound_TextWrapPos{ wrap_local_pos_x }
#endif
	{
	}

	bool Selectable([[maybe_unused]] internal::TextType label, [[maybe_unused]] bool isSelected, [[maybe_unused]] gui::Vec2 size, [[maybe_unused]] FLAG_SELECTABLE flags)
	{
#ifndef IMGUI_DISABLE
		return ImGui::Selectable(label, isSelected, +flags, size);
#else
		return false;
#endif
	}

	bool Checkbox([[maybe_unused]] internal::TextType label, [[maybe_unused]] bool* v)
	{
#ifndef IMGUI_DISABLE
		return ImGui::Checkbox(label, v);
#else
		return false;
#endif
	}

	bool VarDrag([[maybe_unused]] internal::TextType label, [[maybe_unused]] int* v, [[maybe_unused]] float speed, [[maybe_unused]] int min, [[maybe_unused]] int max)
	{
#ifndef IMGUI_DISABLE
		return ImGui::DragInt(label, v, speed, min, max);
#else
		return false;
#endif
	}
	bool VarDrag([[maybe_unused]] internal::TextType label, [[maybe_unused]] unsigned int* v, [[maybe_unused]] float speed, [[maybe_unused]] unsigned int min, [[maybe_unused]] unsigned int max)
	{
#ifndef IMGUI_DISABLE
		return ImGui::DragScalar(label, ImGuiDataType_U32, v, speed, &min, &max);
#else
		return false;
#endif
	}
	bool VarDrag([[maybe_unused]] internal::TextType label, [[maybe_unused]] size_t* v, [[maybe_unused]] float speed, [[maybe_unused]] size_t min, [[maybe_unused]] size_t max)
	{
#ifndef IMGUI_DISABLE
		return ImGui::DragScalar(label, ImGuiDataType_U64, v, speed, &min, &max);
#else
		return false;
#endif
	}
	bool VarDrag([[maybe_unused]] internal::TextType label, [[maybe_unused]] float* v, [[maybe_unused]] float speed, [[maybe_unused]] float min, [[maybe_unused]] float max, [[maybe_unused]] const char* format)
	{
#ifndef IMGUI_DISABLE
		return ImGui::DragFloat(label, v, speed, min, max, format);
#else
		return false;
#endif
	}
	bool VarDrag([[maybe_unused]] internal::TextType label, [[maybe_unused]] double* v, [[maybe_unused]] double speed, [[maybe_unused]] double min, [[maybe_unused]] double max, [[maybe_unused]] const char* format)
	{
#ifndef IMGUI_DISABLE
		// why the fk does imgui support doubles for input but not for dragging??
		float f{ static_cast<float>(*v) };
		bool changed{ ImGui::DragFloat(label, &f, static_cast<float>(speed), static_cast<float>(min), static_cast<float>(max), format) };
		if (changed)
			*v = static_cast<double>(f);
		return changed;
#else
		return false;
#endif
	}

	namespace internal {
		bool DrawFloatComponent(const char* text, const char* label, float* value, float min, float max, float speed, const Vec4& textColor, const char* format)
		{
			{
				SetStyleColor styleColText{ FLAG_STYLE_COLOR::TEXT, textColor };
				Text(text);
			}
			SameLine();
			SetNextItemWidth(GetAvailableContentRegion().x);
			return VarDrag(label, value, speed, min, max, format);
		}
	}

#ifdef GUI_FEATURE_CUSTOM_MATH_VEC_CLASSES
	bool VarDrag([[maybe_unused]] internal::TextTypeExtra label, [[maybe_unused]] internal::ProjectVec2* v, [[maybe_unused]] float speed, [[maybe_unused]] internal::ProjectVec2 min, [[maybe_unused]] internal::ProjectVec2 max, [[maybe_unused]] const char* format)
	{
#ifndef IMGUI_DISABLE
		bool modified = false;
		if (Table table{ label, 3, false, FLAG_TABLE::HIDE_HEADER })
		{
			modified |= internal::DrawFloatComponent("X", "##X", &v->x, min.x, max.x, speed, Vec4{ 1.0f, 0.4f, 0.4f, 1.0f }, format);
			table.NextColumn();
			modified |= internal::DrawFloatComponent("Y", "##Y", &v->y, min.y, max.y, speed, Vec4{ 0.4f, 1.0f, 0.4f, 1.0f }, format);
			table.NextColumn();
			Text(label);
		}
		return modified;
#else
		return false;
#endif
	}

	bool VarDrag([[maybe_unused]] internal::TextTypeExtra label, [[maybe_unused]] internal::ProjectVec3* v, [[maybe_unused]] float speed, [[maybe_unused]] internal::ProjectVec3 min, [[maybe_unused]] internal::ProjectVec3 max, [[maybe_unused]] const char* format)
	{
#ifndef IMGUI_DISABLE
		bool modified = false;
		if (gui::Table table{ label, 4, false, gui::FLAG_TABLE::HIDE_HEADER })
		{
			modified |= internal::DrawFloatComponent("X", "##X", &v->x, min.x, max.x, speed, Vec4{ 1.0f, 0.4f, 0.4f, 1.0f }, format);
			table.NextColumn();
			modified |= internal::DrawFloatComponent("Y", "##Y", &v->y, min.y, max.y, speed, Vec4{ 0.4f, 1.0f, 0.4f, 1.0f }, format);
			table.NextColumn();
			modified |= internal::DrawFloatComponent("Z", "##Z", &v->z, min.z, max.z, speed, Vec4{ 0.4f, 0.4f, 1.0f, 1.0f }, format);
			table.NextColumn();
			Text(label);
		}
		return modified;
#else
		return false;
#endif
	}

	bool VarDrag([[maybe_unused]] internal::TextTypeExtra label, [[maybe_unused]] internal::ProjectVec4* v, [[maybe_unused]] float speed, [[maybe_unused]] internal::ProjectVec4 min, [[maybe_unused]] internal::ProjectVec4 max, [[maybe_unused]] const char* format)
	{
#ifndef IMGUI_DISABLE
		bool modified = false;
		if (gui::Table table{ label, 5, false, gui::FLAG_TABLE::HIDE_HEADER })
		{
			modified |= internal::DrawFloatComponent("X", "##X", &v->x, min.x, max.x, speed, Vec4{ 1.0f, 0.4f, 0.4f, 1.0f }, format);
			table.NextColumn();
			modified |= internal::DrawFloatComponent("Y", "##Y", &v->y, min.y, max.y, speed, Vec4{ 0.4f, 1.0f, 0.4f, 1.0f }, format);
			table.NextColumn();
			modified |= internal::DrawFloatComponent("Z", "##Z", &v->z, min.z, max.z, speed, Vec4{ 0.4f, 0.4f, 1.0f, 1.0f }, format);
			table.NextColumn();
			modified |= internal::DrawFloatComponent("W", "##W", &v->w, min.w, max.w, speed, Vec4{ 0.6f, 0.6f, 0.6f, 1.0f }, format);
			table.NextColumn();
			Text(label);
		}
		return modified;
#else
		return false;
#endif
	}
#endif // GUI_FEATURE_CUSTOM_MATH_VEC_CLASSES

	bool VarInput([[maybe_unused]] internal::TextType label, [[maybe_unused]] int* v, [[maybe_unused]] int step)
	{
#ifndef IMGUI_DISABLE
		return ImGui::InputInt(label, v, step, step * 10);
#else
		return false;
#endif
	}
	bool VarInput([[maybe_unused]] internal::TextType label, [[maybe_unused]] unsigned int* v, [[maybe_unused]] unsigned int step)
	{
#ifndef IMGUI_DISABLE
		unsigned int fastStep{ step * 10 };
		return ImGui::InputScalar(label, ImGuiDataType_U32, v, &step, &fastStep);
#else
		return false;
#endif
	}
	bool VarInput([[maybe_unused]] internal::TextType label, [[maybe_unused]] size_t* v, [[maybe_unused]] size_t step)
	{
#ifndef IMGUI_DISABLE
		size_t fastStep{ step * 10 };
		return ImGui::InputScalar(label, ImGuiDataType_U64, v, &step, &fastStep);
#else
		return false;
#endif
	}
	bool VarInput([[maybe_unused]] internal::TextType label, [[maybe_unused]] float* v, [[maybe_unused]] float step)
	{
#ifndef IMGUI_DISABLE
		return ImGui::InputFloat(label, v, step, step * 10.0f);
#else
		return false;
#endif
	}
	bool VarInput([[maybe_unused]] internal::TextType label, [[maybe_unused]] double* v, [[maybe_unused]] double step)
	{
#ifndef IMGUI_DISABLE
		return ImGui::InputDouble(label, v, step, step * 10.0);
#else
		return false;
#endif
	}
#ifdef GUI_FEATURE_CUSTOM_MATH_VEC_CLASSES
	bool VarInput([[maybe_unused]] internal::TextType label, [[maybe_unused]] internal::ProjectVec2* v)
	{
#ifndef IMGUI_DISABLE
		return ImGui::InputFloat2(label, &v->x);
#else
		return false;
#endif
	}
	bool VarInput([[maybe_unused]] internal::TextType label, [[maybe_unused]] internal::ProjectVec3* v)
	{
#ifndef IMGUI_DISABLE
		return ImGui::InputFloat3(label, &v->x);
#else
		return false;
#endif
	}
	bool VarInput([[maybe_unused]] internal::TextType label, [[maybe_unused]] internal::ProjectVec4* v)
	{
#ifndef IMGUI_DISABLE
		return ImGui::InputFloat4(label, &v->x);
#else
		return false;
#endif
	}

	bool VarColor([[maybe_unused]] internal::TextType label, [[maybe_unused]] internal::ProjectVec3* v, [[maybe_unused]] FLAG_COLOR_EDIT flags)
	{
#ifndef IMGUI_DISABLE
		return ImGui::ColorEdit3(label, &v->x, +flags);
#else
		return false;
#endif
	}
	bool VarColor([[maybe_unused]] internal::TextType label, [[maybe_unused]] internal::ProjectVec4* v, [[maybe_unused]] FLAG_COLOR_EDIT flags)
	{
#ifndef IMGUI_DISABLE
		return ImGui::ColorEdit4(label, &v->x, +flags);
#else
		return false;
#endif
	}
#endif // GUI_FEATURE_CUSTOM_MATH_VEC_CLASSES
#ifndef IMGUI_DISABLE // This overload is specifically for ImGui's vec. For ImGui disabled scenarios, this would cause a re-definition of the above function.
	bool VarColor([[maybe_unused]] internal::TextType label, [[maybe_unused]] gui::Vec4* v, [[maybe_unused]] FLAG_COLOR_EDIT flags)
	{
		return ImGui::ColorEdit4(label, &v->x, +flags);
	}
#endif // IMGUI_DISABLE

	bool VarSlider([[maybe_unused]] internal::TextType label, [[maybe_unused]] int* v, [[maybe_unused]] int min, [[maybe_unused]] int max)
	{
#ifndef IMGUI_DISABLE
		return ImGui::SliderInt(label, v, min, max);
#else
		return false;
#endif
	}

	bool VarSlider([[maybe_unused]] internal::TextType label, [[maybe_unused]] float* v, [[maybe_unused]] float min, [[maybe_unused]] float max)
	{
#ifndef IMGUI_DISABLE
		return ImGui::SliderFloat(label, v, min, max);
#else
		return false;
#endif
	}

	bool VarDefault(internal::TextType label, bool* v)
	{
		return Checkbox(label, v);
	}
	bool VarDefault(internal::TextType label, char* v)
	{
		char prevVal{ *v };
		TextBox(label, v, 1);
		return *v != prevVal;
	}
	bool VarDefault(internal::TextType label, int* v)
	{
		return VarInput(label, v);
	}
	bool VarDefault(internal::TextType label, unsigned int* v)
	{
		return VarInput(label, v);
	}
	bool VarDefault(internal::TextType label, size_t* v)
	{
		return VarInput(label, v);
	}
	bool VarDefault(internal::TextType label, float* v)
	{
		return VarDrag(label, v);
	}
	bool VarDefault(internal::TextType label, double* v)
	{
		return VarDrag(label, v);
	}
#ifdef GUI_FEATURE_CUSTOM_MATH_VEC_CLASSES
	bool VarDefault(internal::TextTypeForFormatting label, internal::ProjectVec2* v)
	{
		return VarInput(label, v);
	}
	bool VarDefault(internal::TextTypeForFormatting label, internal::ProjectVec3* v)
	{
		return VarInput(label, v);
	}
	bool VarDefault(internal::TextTypeForFormatting label, internal::ProjectVec4* v)
	{
		return VarInput(label, v);
	}
#endif // GUI_FEATURE_CUSTOM_MATH_VEC_CLASSES
	bool VarDefault(internal::TextType label, std::string* v)
	{
		return TextBox(label, v);
	}

	bool& GetVar([[maybe_unused]] internal::TextType label, [[maybe_unused]] bool defaultVal)
	{
#ifndef IMGUI_DISABLE
		return *ImGui::GetStateStorage()->GetBoolRef(ImGui::GetID(label), defaultVal);
#else
		static bool dummy{};
		return dummy;
#endif
	}
	int& GetVar([[maybe_unused]] internal::TextType label, [[maybe_unused]] int defaultVal)
	{
#ifndef IMGUI_DISABLE
		return *ImGui::GetStateStorage()->GetIntRef(ImGui::GetID(label), defaultVal);
#else
		static int dummy{};
		return dummy;
#endif
	}
	float& GetVar([[maybe_unused]] internal::TextType label, [[maybe_unused]] float defaultVal)
	{
#ifndef IMGUI_DISABLE
		return *ImGui::GetStateStorage()->GetFloatRef(ImGui::GetID(label), defaultVal);
#else
		static float dummy{};
		return dummy;
#endif
	}

	bool Button([[maybe_unused]] internal::TextType label, [[maybe_unused]] const Vec2& size)
	{
#ifndef IMGUI_DISABLE
		return ImGui::Button(label, size);
#else
		return false;
#endif
	}

	bool SmallButton([[maybe_unused]] internal::TextType label)
	{
#ifndef IMGUI_DISABLE
		return ImGui::SmallButton(label);
#else
		return false;
#endif
	}

	bool ImageButton([[maybe_unused]] internal::TextType label, [[maybe_unused]] TextureID textureID, [[maybe_unused]] Vec2 size, [[maybe_unused]] Vec2 uv0, [[maybe_unused]] Vec2 uv1)
	{
#ifndef IMGUI_DISABLE
		return ImGui::ImageButton(label, textureID, size, uv0, uv1);
#else
		return false;
#endif
	}

	bool InvisibleButton([[maybe_unused]] internal::TextType label, [[maybe_unused]] Vec2 size)
	{
#ifndef IMGUI_DISABLE
		return ImGui::InvisibleButton(label, size);
#else
		return false;
#endif
	}

	template <>
	void PayloadSource<std::string>::SetPayloadTarget([[maybe_unused]] internal::TextType identifier, [[maybe_unused]] const std::string& data)
	{
#ifndef IMGUI_DISABLE
		ImGui::SetDragDropPayload(identifier, data.c_str(), data.size() + 1);
#endif
	}

	Combo::Combo([[maybe_unused]] internal::TextType label, [[maybe_unused]] internal::TextType previewValue, [[maybe_unused]] FLAG_COMBO flags)
#ifndef IMGUI_DISABLE
		: shouldCallEndCombo{ ImGui::BeginCombo(label, previewValue, +flags) }
#else
		: shouldCallEndCombo{ false }
#endif
	{
	}

	Combo::Combo(internal::TextType label, const char* const* data, size_t numElems, int* selectedIndex)
		: shouldCallEndCombo{ false }
	{
		CallCombo(label, data, numElems, selectedIndex);
	}

	Combo::~Combo()
	{
#ifndef IMGUI_DISABLE
		if (shouldCallEndCombo)
			ImGui::EndCombo();
#endif
	}

	bool Combo::Selectable([[maybe_unused]] internal::TextType label, [[maybe_unused]] bool isSelected)
	{
#ifndef IMGUI_DISABLE
		return ImGui::Selectable(label, isSelected);
#else
		return false;
#endif
	}

	Combo::operator bool() const
	{
		return selectionHappened || shouldCallEndCombo;
	}

	void Combo::CallCombo([[maybe_unused]] internal::TextType label, [[maybe_unused]] const char* const* data, [[maybe_unused]] size_t numElems, [[maybe_unused]] int* selectedIndex)
	{
#ifndef IMGUI_DISABLE
		selectionHappened = ImGui::Combo(label, selectedIndex, data, static_cast<int>(numElems));
#endif
	}

	Menu::Menu([[maybe_unused]] internal::TextType label)
#ifndef IMGUI_DISABLE
		: internal::BeginEndBound_Menu{ label, true }
#endif
	{
	}

	bool Menu::Item(internal::TextType label, bool* p_selected)
	{
		return MenuItem(label, p_selected);
	}

	ItemContextMenu::ItemContextMenu([[maybe_unused]] internal::TextType label, [[maybe_unused]] FLAG_POPUP flags)
#ifndef IMGUI_DISABLE
		: internal::BeginEndBound_ItemContextMenu{ label, +flags }
#endif
	{
	}

	bool ItemContextMenu::Item(internal::TextType label, bool* p_selected)
	{
		return MenuItem(label, p_selected);
	}

	bool MenuItem([[maybe_unused]] internal::TextType label, [[maybe_unused]] bool* p_selected)
	{
#ifndef IMGUI_DISABLE
		return ImGui::MenuItem(label, nullptr, p_selected);
#else
		return false;
#endif
	}

	TreeNode::TreeNode(internal::TextType label, bool isLeaf, FLAG_TREE_NODE flags)
#ifndef IMGUI_DISABLE
		: internal::BeginEndBound_TreeNode{ label, +(flags | (isLeaf ? FLAG_TREE_NODE::LEAF : FLAG_TREE_NODE::NONE)) }
#endif
	{
	}

	CollapsingHeader::CollapsingHeader([[maybe_unused]] internal::TextType label, [[maybe_unused]] FLAG_TREE_NODE flags)
#ifndef IMGUI_DISABLE
		: internal::BeginEndBound_CollapsingHeader{ label, +flags }
#endif
	{
	}

	Table::Table([[maybe_unused]] internal::TextType label, [[maybe_unused]] int numCols, bool hasHeader, [[maybe_unused]] FLAG_TABLE flags, [[maybe_unused]] const Vec2& outerSize, [[maybe_unused]] float innerWidth)
		: tableFlags{ flags }
#ifndef IMGUI_DISABLE
		, internal::BeginEndBound_Table{ label, numCols, +flags, outerSize, innerWidth }
#endif
	{
		// ImGui requires TableNextColumn() to be called before data of first column.
		if (!hasHeader)
			NextColumn();
	}

	void Table::AddColumnHeader([[maybe_unused]] internal::TextType label, [[maybe_unused]] FLAG_TABLE_COLUMN flags, [[maybe_unused]] float width)
	{
#ifndef IMGUI_DISABLE
		ImGui::TableSetupColumn(label, +flags, width);
#endif
	}

	void Table::SubmitColumnHeaders()
	{
#ifndef IMGUI_DISABLE
		if (!+(tableFlags & FLAG_TABLE::HIDE_HEADER))
			ImGui::TableHeadersRow();
		NextColumn();
#endif
	}

	void Table::NextColumn()
	{
#ifndef IMGUI_DISABLE
		ImGui::TableNextColumn();
#endif
	}

	Indent::Indent(float amt)
		: indentAmt{ amt }
	{
#ifndef IMGUI_DISABLE
		ImGui::Indent(indentAmt);
#endif
	}

	Indent::~Indent()
	{
#ifndef IMGUI_DISABLE
		ImGui::Unindent(indentAmt);
#endif
	}

	void Separator()
	{
#ifndef IMGUI_DISABLE
		ImGui::Separator();
#endif
	}

	void Spacing()
	{
#ifndef IMGUI_DISABLE
		ImGui::Spacing();
#endif
	}

	void SameLine([[maybe_unused]] float offset_from_start_x, [[maybe_unused]] float spacing)
	{
#ifndef IMGUI_DISABLE
		ImGui::SameLine(offset_from_start_x, spacing);
#endif
	}

	Vec2 GetAvailableContentRegion()
	{
#ifndef IMGUI_DISABLE
		return ImGui::GetContentRegionAvail();
#else
		return Vec2{ 1.0f, 1.0f };
#endif
	}

	Vec2 GetPrevItemRectSize()
	{
#ifndef IMGUI_DISABLE
		return ImGui::GetItemRectSize();
#else
		return Vec2{ 1.0f, 1.0f };
#endif
	}
	Vec2 GetPrevItemRectMin()
	{
#ifndef IMGUI_DISABLE
		return ImGui::GetItemRectMin();
#else
		return Vec2{ 1.0f, 1.0f };
#endif
	}
	Vec2 GetPrevItemRectMax()
	{
#ifndef IMGUI_DISABLE
		return ImGui::GetItemRectMax();
#else
		return Vec2{ 1.0f, 1.0f };
#endif
	}

	void SetNextItemWidth([[maybe_unused]] float width)
	{
#ifndef IMGUI_DISABLE
		ImGui::SetNextItemWidth(width);
#endif
	}

	float GetTextLineHeightWithSpacing()
	{
#ifndef IMGUI_DISABLE
		return ImGui::GetTextLineHeightWithSpacing();
#else
		return 0.0f;
#endif
	}

	float GetWindowWidth()
	{
#ifndef IMGUI_DISABLE
		return ImGui::GetWindowWidth();
#else
		return 0.0f;
#endif
	}

	float GetWindowHeight()
	{
#ifndef IMGUI_DISABLE
		return ImGui::GetWindowHeight();
#else
		return 0.0f;
#endif
	}

	Vec2 GetWindowContentRegionMax()
	{
#ifndef IMGUI_DISABLE
		return ImGui::GetWindowContentRegionMax();
#else
		return Vec2{ 1.0f, 1.0f };
#endif
	}

	void SetNextWindowPos([[maybe_unused]] const Vec2& pos, [[maybe_unused]] gui::FLAG_COND flags, [[maybe_unused]] gui::Vec2 pivot)
	{
#ifndef IMGUI_DISABLE
		ImGui::SetNextWindowPos(pos, +flags, pivot);
#endif
	}
	void SetNextWindowSize([[maybe_unused]] const Vec2& size, [[maybe_unused]] gui::FLAG_COND flags)
	{
#ifndef IMGUI_DISABLE
		ImGui::SetNextWindowSize(size, +flags);
#endif
	}

	void SetNextWindowSizeConstraints([[maybe_unused]] const Vec2& min, [[maybe_unused]] const Vec2& max)
	{
#ifndef IMGUI_DISABLE
		ImGui::SetNextWindowSizeConstraints(min, max);
#endif
	}

	void SetNextWindowBackgroundAlpha([[maybe_unused]] float alpha)
	{
#ifndef IMGUI_DISABLE
		ImGui::SetNextWindowBgAlpha(alpha);
#endif
	}

	void BringCurrWindowToFront()
	{
#ifndef IMGUI_DISABLE
		ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
#endif
	}

	SetStyleColor::SetStyleColor([[maybe_unused]] FLAG_STYLE_COLOR idx, [[maybe_unused]] const Vec4& val, [[maybe_unused]] bool apply)
#ifndef IMGUI_DISABLE
		: internal::BeginEndBound_StyleColor{ apply, +idx, val }
#endif
	{
	}

	UnsetStyleColor::UnsetStyleColor([[maybe_unused]] unsigned int numPops)
	{
#ifndef IMGUI_DISABLE
		auto context{ ImGui::GetCurrentContext() };
		for (; numPops; --numPops)
		{
			popped.push_back(context->ColorStack.back());
			ImGui::PopStyleColor();
		}
#endif
	}

	UnsetStyleColor::~UnsetStyleColor()
	{
#ifndef IMGUI_DISABLE
		while (!popped.empty())
		{
			const auto& styleCol{ popped.back() };
			ImGui::PushStyleColor(styleCol.Col, styleCol.BackupValue);
			popped.pop_back();
		}
#endif
	}

	SetStyleVar::SetStyleVar([[maybe_unused]] FLAG_STYLE_VAR idx, [[maybe_unused]] const Vec2& val)
#ifndef IMGUI_DISABLE
		: internal::BeginEndBound_StyleVar{ +idx, val }
#endif
	{
	}

	UnsetStyleVar::UnsetStyleVar([[maybe_unused]] unsigned int numPops)
	{
#ifndef IMGUI_DISABLE
		auto context{ ImGui::GetCurrentContext() };
		for (; numPops; --numPops)
		{
			popped.push_back(context->StyleVarStack.back());
			ImGui::PopStyleVar();
		}
#endif
	}

	UnsetStyleVar::~UnsetStyleVar()
	{
#ifndef IMGUI_DISABLE
		while (!popped.empty())
		{
			const auto& styleVar{ popped.back() };
			ImGui::PushStyleVar(styleVar.VarIdx, Vec2{ styleVar.BackupFloat[0], styleVar.BackupFloat[1] });
			popped.pop_back();
		}
#endif
	}

	Vec4 GetStyleColor([[maybe_unused]] FLAG_STYLE_COLOR style)
	{
#ifndef IMGUI_DISABLE
		return ImGui::GetStyleColorVec4(+style);
#else
		return Vec4{};
#endif
	}

	Vec4 GetStyleColor(FLAG_STYLE_COLOR style, float channelMult, float alphaMult)
	{
		Vec4 styleColor{ GetStyleColor(style) };
		styleColor.x *= channelMult;
		styleColor.y *= channelMult;
		styleColor.z *= channelMult;
		styleColor.w *= alphaMult;
		return styleColor;
	}

	Vec2 GetStyleFramePadding()
	{
#ifndef IMGUI_DISABLE
		return ImGui::GetStyle().FramePadding;
#else
		return Vec2{};
#endif
	}

	void Image([[maybe_unused]] TextureID textureID, [[maybe_unused]] Vec2 size)
	{
#ifndef IMGUI_DISABLE
		ImGui::Image(textureID, size);
#endif
	}

	Vec2 GetScreenCursorPos()
	{
#ifndef IMGUI_DISABLE
		return ImGui::GetCursorScreenPos();
#else
		return Vec2{};
#endif
	}

	void SetDrawCursorPosX([[maybe_unused]] float x)
	{
#ifndef IMGUI_DISABLE
		ImGui::SetCursorPosX(x);
#endif
	}
	void SetDrawCursorPosY([[maybe_unused]] float y)
	{
#ifndef IMGUI_DISABLE
		ImGui::SetCursorPosY(y);
#endif
	}
	void SetDrawCursorPos([[maybe_unused]] Vec2 pos)
	{
#ifndef IMGUI_DISABLE
		ImGui::SetCursorPos(pos);
#endif
	}

	void DrawLine([[maybe_unused]] Vec2 p0, [[maybe_unused]] Vec2 p1, [[maybe_unused]] const Vec4& color)
	{
#ifndef IMGUI_DISABLE
		ImGui::GetWindowDrawList()->AddLine(p0, p1, ImGui::ColorConvertFloat4ToU32(color));
#endif
	}

	void DrawTriangle([[maybe_unused]] Vec2 p0, [[maybe_unused]] Vec2 p1, [[maybe_unused]] Vec2 p2, [[maybe_unused]] const Vec4& color)
	{
#ifndef IMGUI_DISABLE
		ImGui::GetWindowDrawList()->AddTriangleFilled(p0, p1, p2, ImGui::ColorConvertFloat4ToU32(color));
#endif
	}

	void DrawRect([[maybe_unused]] Vec2 p0, [[maybe_unused]] Vec2 p1, [[maybe_unused]] const Vec4& color, [[maybe_unused]] float cornerRoundingAmt)
	{
#ifndef IMGUI_DISABLE
		ImGui::GetWindowDrawList()->AddRectFilled(p0, p1, ImGui::ColorConvertFloat4ToU32(color), cornerRoundingAmt);
#endif
	}

	void DrawCircle([[maybe_unused]] Vec2 center, [[maybe_unused]] float radius, [[maybe_unused]] const Vec4& color)
	{
#ifndef IMGUI_DISABLE
		ImGui::GetWindowDrawList()->AddCircleFilled(center, radius, ImGui::ColorConvertFloat4ToU32(color));
#endif
	}

	void DrawText([[maybe_unused]] internal::TextType text, Vec2 pos, [[maybe_unused]] const Vec4& color)
	{
#ifndef IMGUI_DISABLE
		ImGui::GetWindowDrawList()->AddText(pos, ImGui::ColorConvertFloat4ToU32(color), text);
#endif
	}

	Disabled::Disabled([[maybe_unused]] bool isDisabled)
#ifndef IMGUI_DISABLE
		: internal::BeginEndBound_Disabled{ isDisabled }
#endif
	{
	}

	Enabled::Enabled([[maybe_unused]] bool isEnabled)
#ifndef IMGUI_DISABLE
		: didEnable{ isEnabled && ImGui::GetCurrentContext()->DisabledStackSize > 0 && (ImGui::GetCurrentContext()->CurrentItemFlags & ImGuiItemFlags_Disabled) }
#else
		: didEnable{}
#endif
	{
#ifndef IMGUI_DISABLE
		if (didEnable)
			ImGui::EndDisabled();
#endif
	}

	Enabled::~Enabled()
	{
#ifndef IMGUI_DISABLE
		if (didEnable)
			ImGui::BeginDisabled();
#endif
	}

	bool IsKeyPressed([[maybe_unused]] KEY key, [[maybe_unused]] bool repeating)
	{
#ifndef IMGUI_DISABLE
		return ImGui::IsKeyPressed(static_cast<ImGuiKey>(key), repeating);
#else
		return false;
#endif
	}

	void SetKeyboardFocusHere([[maybe_unused]] int offset)
	{
#ifndef IMGUI_DISABLE
		ImGui::SetKeyboardFocusHere(offset);
#endif
	}

	bool IsMouseClicked([[maybe_unused]] MOUSE_BUTTON button)
	{
#ifndef IMGUI_DISABLE
		return ImGui::IsMouseClicked(+button);
#else
		return false;
#endif
	}

	void SetScrollHereY([[maybe_unused]] float center_y_ratio)
	{
#ifndef IMGUI_DISABLE
		ImGui::SetScrollHereY(center_y_ratio);
#endif
	}

	Vec2 GetMainViewportPos()
	{
#ifndef IMGUI_DISABLE
		return ImGui::GetMainViewport()->Pos;
#else
		return Vec2{};
#endif
	}

	Vec2 GetMainViewportSize()
	{
#ifndef IMGUI_DISABLE
		return ImGui::GetMainViewport()->Size;
#else
		return Vec2{};
#endif
	}
}
