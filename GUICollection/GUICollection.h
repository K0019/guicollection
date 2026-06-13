#pragma once

// GUICollection v0.1.0
//
// This library provides a collection of structures and functions wrapping ImGui,
// with the goal of fixing some legacy design decisions while bringing modern syntax
// and programming patterns to our ImGui interfacing code.
//
// The major difference that this library offers is the heavy usage of RAII that
// let's us more easily visualize stuff like whether that button is colored or not
// based on the lifetime of class objects rather than hard to track Begin()/End()
// blocks. No more losing track of whether the PushStyleColor() call has already
// been popped by PopStyleColor(), or whether you popped the correct number of times,
// or whether you remembered to call End() outside of the Begin() if block!
//
// Big disclaimer though: I wouldn't call this library stable, if ever. Ultimately
// this is just a personal project born from the hatred of my own ImGui code, so my
// goal here is to simply create a clean and easy to use interface that encourages
// clean code and which also maintains my sanity. If the stars align, I hope this
// library can help you out too!
//
// Of course, this wouldn't have been possible without ImGui existing in the first
// place. Huge props to @ocornut for their wonderful creation and incredible
// implementation. Legit no idea how you manage with all the possible customization.
// https://github.com/ocornut/imgui



// Enable/Disable all usage of ImGui
//#define IMGUI_DISABLE							// Intended for if you need to remove all ImGui references but don't want to refactor your project code that's already coded for ImGui, as long as your interfaces are via this library. As for why you would ever need this, well iykyk :)

// Enable/Disable features
#define GUI_FEATURE_CUSTOM_MATH_VEC_CLASSES		// Adds support for your custom math vector classes to this library. If IMGUI_DISABLE is defined, this needs to be enabled - I needs SOME sort of vec class, you know lol.
#define GUI_FEATURE_CPP20_FORMAT				// Use std::format style rather than printf style for format strings

// Enable/Disable extensions
#define GUI_EXTENSION_NOTIFICATIONS				// Implements notification popups at the bottom-right corner

// Customize features
#ifdef GUI_FEATURE_CUSTOM_MATH_VEC_CLASSES
#include <glm/glm.hpp>									 // Your math header with vec2/3/4 structs defined
#define GUI_CUSTOM_MATH_VEC_CLASSES_VEC2_TYPE glm::vec2; // With members x, y in that order
#define GUI_CUSTOM_MATH_VEC_CLASSES_VEC3_TYPE glm::vec3; // With members x, y, z in that order
#define GUI_CUSTOM_MATH_VEC_CLASSES_VEC4_TYPE glm::vec4; // With members x, y, z, w in that order
#endif // GUI_FEATURE_CUSTOM_MATH_VEC_CLASSES



#ifndef IMGUI_DISABLE
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include "imgui_internal.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#endif // IMGUI_DISABLE

#include <string>
#include <ranges>
#include "MacroTemplates.h"
#include "Utilities.h"

#ifdef GUI_FEATURE_CPP20_FORMAT
#include <format>
#endif // GUI_FEATURE_CPP20_FORMAT



namespace gui {

#pragma region Forward Declaration

	namespace internal {
		struct TextTypeExtra;
	}

#pragma endregion // Forward Declaration

#pragma region Types

#ifdef GUI_FEATURE_CUSTOM_MATH_VEC_CLASSES
	namespace internal {
		using ProjectVec2 = GUI_CUSTOM_MATH_VEC_CLASSES_VEC2_TYPE;
		using ProjectVec3 = GUI_CUSTOM_MATH_VEC_CLASSES_VEC3_TYPE;
		using ProjectVec4 = GUI_CUSTOM_MATH_VEC_CLASSES_VEC4_TYPE;
	}
#endif // GUI_FEATURE_CUSTOM_MATH_VEC_CLASSES

#ifndef IMGUI_DISABLE
	using Vec2 = ImVec2;
	using Vec4 = ImVec4;
	using TextureID = ImTextureID;
#else
	using Vec2 = internal::ProjectVec2;
	using Vec4 = internal::ProjectVec4;
	using TextureID = std::uintptr_t;
#endif

	namespace types
	{
#ifndef IMGUI_DISABLE
		using InputTextCallbackData = ImGuiInputTextCallbackData;
		using InputTextCallback = ImGuiInputTextCallback;
#else
		using InputTextCallbackData = struct {};
		using InputTextCallback = int(*)(InputTextCallbackData*);
#endif
	}

#pragma endregion // Types

#pragma region Flags

	//! ImGuiKey
	// TODO: Finish this
#define GUICOLLECTION_KEY \
	X(UP, ImGuiKey_UpArrow) \
	X(DOWN, ImGuiKey_DownArrow) \
	X(ENTER, ImGuiKey_Enter) \
	X(ESC, ImGuiKey_Escape) \
	X(GRAVE, ImGuiKey_GraveAccent)

#define GUICOLLECTION_MOUSE_BUTTON \
	X(LEFT, ImGuiMouseButton_Left) \
	X(RIGHT, ImGuiMouseButton_Right) \
	X(MIDDLE, ImGuiMouseButton_Middle)

	//! ImGuiWindowFlags
#define GUICOLLECTION_FLAG_WINDOW \
	X(NONE, ImGuiWindowFlags_None) \
	X(NO_TITLE_BAR, ImGuiWindowFlags_NoTitleBar) \
	X(NO_RESIZE, ImGuiWindowFlags_NoResize) \
	X(NO_MOVE, ImGuiWindowFlags_NoMove) \
	X(NO_SCROLL_BAR, ImGuiWindowFlags_NoScrollbar) \
	X(NO_SCROLL_WITH_MOUSE, ImGuiWindowFlags_NoScrollWithMouse) \
	X(NO_COLLAPSE, ImGuiWindowFlags_NoCollapse) \
	X(ALWAYS_AUTO_RESIZE, ImGuiWindowFlags_AlwaysAutoResize) \
	X(NO_BACKGROUND, ImGuiWindowFlags_NoBackground) \
	X(NO_SAVED_SETTINGS, ImGuiWindowFlags_NoSavedSettings) \
	X(NO_MOUSE_INPUTS, ImGuiWindowFlags_NoMouseInputs) \
	X(HAS_MENU_BAR, ImGuiWindowFlags_MenuBar) \
	X(HORIZONTAL_SCROLL_BAR, ImGuiWindowFlags_HorizontalScrollbar) \
	X(NO_FOCUS_ON_APPEARING, ImGuiWindowFlags_NoFocusOnAppearing) \
	X(NO_BRING_TO_FRONT_ON_FOCUS, ImGuiWindowFlags_NoBringToFrontOnFocus) \
	X(ALWAYS_VERTICAL_SCROLL_BAR, ImGuiWindowFlags_AlwaysVerticalScrollbar) \
	X(ALWAYS_HORIZONTAL_SCROLL_BAR, ImGuiWindowFlags_AlwaysHorizontalScrollbar) \
	X(NO_NAV_INPUTS, ImGuiWindowFlags_NoNavInputs) \
	X(NO_NAV_FOCUS, ImGuiWindowFlags_NoNavFocus) \
	X(UNSAVED_DOCUMENT, ImGuiWindowFlags_UnsavedDocument) \
	X_DEPEND_DOCKING(NO_DOCKING, ImGuiWindowFlags_NoDocking) \
	X(NO_NAV, NO_NAV_INPUTS | NO_NAV_FOCUS) \
	X(NO_DECORATION, NO_TITLE_BAR | NO_RESIZE | NO_SCROLL_BAR | NO_COLLAPSE) \
	X(NO_INPUTS, NO_MOUSE_INPUTS | NO_NAV_INPUTS | NO_NAV_FOCUS)

	//! ImGuiChildFlags
#define GUICOLLECTION_FLAG_CHILD \
	X(NONE, ImGuiChildFlags_None) \
	X(BORDERS, ImGuiChildFlags_Borders) \
	X(ALWAYS_USE_WINDOW_PADDING, ImGuiChildFlags_AlwaysUseWindowPadding) \
	X(RESIZE_X, ImGuiChildFlags_ResizeX) \
	X(RESIZE_Y, ImGuiChildFlags_ResizeY) \
	X(AUTO_RESIZE_X, ImGuiChildFlags_AutoResizeX) \
	X(AUTO_RESIZE_Y, ImGuiChildFlags_AutoResizeY) \
	X(ALWAYS_AUTO_RESIZE, ImGuiChildFlags_AlwaysAutoResize) \
	X(FRAME_STYLE, ImGuiChildFlags_FrameStyle)

	//! ImGuiInputTextFlags
#define GUICOLLECTION_FLAG_INPUT_TEXT \
	X(NONE, ImGuiInputTextFlags_None) \
	X(CHARS_DECIMAL, ImGuiInputTextFlags_CharsDecimal) \
	X(CHARS_HEXADECIMAL, ImGuiInputTextFlags_CharsHexadecimal) \
	X(CHARS_SCIENTIFIC, ImGuiInputTextFlags_CharsScientific) \
	X(CHARS_UPPERCASE, ImGuiInputTextFlags_CharsUppercase) \
	X(CHARS_NO_BLANK, ImGuiInputTextFlags_CharsNoBlank) \
	X(ALLOW_TAB_INPUT, ImGuiInputTextFlags_AllowTabInput) \
	X(ENTER_RETURNS_TRUE, ImGuiInputTextFlags_EnterReturnsTrue) \
	X(ESCAPE_CLEARS_ALL, ImGuiInputTextFlags_EscapeClearsAll) \
	X(CTRL_ENTER_FOR_NEW_LINE, ImGuiInputTextFlags_CtrlEnterForNewLine) \
	X(READ_ONLY, ImGuiInputTextFlags_ReadOnly) \
	X(PASSWORD, ImGuiInputTextFlags_Password) \
	X(ALWAYS_OVERWRITE, ImGuiInputTextFlags_AlwaysOverwrite) \
	X(AUTO_SELECT_ALL, ImGuiInputTextFlags_AutoSelectAll) \
	X(PARSE_EMPTY_REF_VAL, ImGuiInputTextFlags_ParseEmptyRefVal) \
	X(DISPLAY_EMPTY_REF_VAL, ImGuiInputTextFlags_DisplayEmptyRefVal) \
	X(NO_HORIZONTAL_SCROLL, ImGuiInputTextFlags_NoHorizontalScroll) \
	X(NO_UNDO_REDO, ImGuiInputTextFlags_NoUndoRedo) \
	X(CALLBACK_COMPLETION, ImGuiInputTextFlags_CallbackCompletion) \
	X(CALLBACK_HISTORY, ImGuiInputTextFlags_CallbackHistory) \
	X(CALLBACK_ALWAYS, ImGuiInputTextFlags_CallbackAlways) \
	X(CALLBACK_CHAR_FILTER, ImGuiInputTextFlags_CallbackCharFilter) \
	X(CALLBACK_RESIZE, ImGuiInputTextFlags_CallbackResize) \
	X(CALLBACK_EDIT, ImGuiInputTextFlags_CallbackEdit)

	//! ImGuiTreeNodeFlags
#define GUICOLLECTION_FLAG_TREE_NODE \
	X(NONE, ImGuiTreeNodeFlags_None) \
	X(SELECTED, ImGuiTreeNodeFlags_Selected) \
	X(FRAMED, ImGuiTreeNodeFlags_Framed) \
	X(ALLOW_OVERLAP, ImGuiTreeNodeFlags_AllowOverlap) \
	X(NO_TREE_PUSH_ON_OPEN, ImGuiTreeNodeFlags_NoTreePushOnOpen) \
	X(NO_AUTO_OPEN_ON_LOG, ImGuiTreeNodeFlags_NoAutoOpenOnLog) \
	X(DEFAULT_OPEN, ImGuiTreeNodeFlags_DefaultOpen) \
	X(OPEN_ON_DOUBLE_CLICK, ImGuiTreeNodeFlags_OpenOnDoubleClick) \
	X(OPEN_ON_ARROW, ImGuiTreeNodeFlags_OpenOnArrow) \
	X(LEAF, ImGuiTreeNodeFlags_Leaf) \
	X(BULLET, ImGuiTreeNodeFlags_Bullet) \
	X(FRAME_PADDING, ImGuiTreeNodeFlags_FramePadding) \
	X(SPAN_AVAILABLE_WIDTH, ImGuiTreeNodeFlags_SpanAvailWidth) \
	X(SPAN_FULL_WIDTH, ImGuiTreeNodeFlags_SpanFullWidth) \
	X(SPAN_TEXT_WIDTH, ImGuiTreeNodeFlags_SpanTextWidth) \
	X(SPAN_ALL_COLUMNS, ImGuiTreeNodeFlags_SpanAllColumns) \
	X(NAV_LEFT_JUMPS_BACK_HERE, ImGuiTreeNodeFlags_NavLeftJumpsBackHere) \
	X(COLLAPSING_HEADER, ImGuiTreeNodeFlags_CollapsingHeader)

	//! ImGuiPopupFlags
#define GUICOLLECTION_FLAG_POPUP \
	X(NONE, ImGuiPopupFlags_None) \
	X(MOUSE_LEFT, ImGuiPopupFlags_MouseButtonLeft) \
	X(MOUSE_RIGHT, ImGuiPopupFlags_MouseButtonRight) \
	X(MOUSE_MIDDLE, ImGuiPopupFlags_MouseButtonMiddle) \
	X(MOUSE_ANY, ImGuiPopupFlags_MouseButtonMask_) \
	X(NO_REOPEN, ImGuiPopupFlags_NoReopen) \
	X(NO_OPEN_OVER_EXISTING_POPUP, ImGuiPopupFlags_NoOpenOverExistingPopup) \
	X(NO_OPEN_OVER_ITEMS, ImGuiPopupFlags_NoOpenOverItems) \
	X(ANY_POPUP_ID, ImGuiPopupFlags_AnyPopupId) \
	X(ANY_POPUP_LEVEL, ImGuiPopupFlags_AnyPopupLevel) \
	X(ANY_POPUP, ImGuiPopupFlags_AnyPopup)

	//! ImGuiDragDropFlags
#define GUICOLLECTION_FLAG_PAYLOAD_SOURCE \
	X(NONE, ImGuiDragDropFlags_None) \
	X(NO_PREVIEW_TOOLTIP, ImGuiDragDropFlags_SourceNoPreviewTooltip) \
	X(NO_DISABLE_HOVER, ImGuiDragDropFlags_SourceNoDisableHover) \
	X(NO_HOLD_TO_OPEN_OTHERS, ImGuiDragDropFlags_SourceNoHoldToOpenOthers) \
	X(ALLOW_NULL_ID, ImGuiDragDropFlags_SourceAllowNullID) \
	X(EXTERNAL_SOURCE, ImGuiDragDropFlags_SourceExtern)
#define GUICOLLECTION_FLAG_PAYLOAD_TARGET \
	X(NONE, ImGuiDragDropFlags_None) \
	X(SUCCESS_BEFORE_DELIVERY, ImGuiDragDropFlags_AcceptBeforeDelivery) \
	X(NO_DRAW_DEFAULT_RECT, ImGuiDragDropFlags_AcceptNoDrawDefaultRect) \
	X(NO_PREVIEW_TOOLTIP, ImGuiDragDropFlags_AcceptNoPreviewTooltip) \
	X(PEEK_ONLY, ImGuiDragDropFlags_AcceptPeekOnly)

	//! ImGuiSelectableFlags
#define GUICOLLECTION_FLAG_SELECTABLE \
	X(NONE, ImGuiSelectableFlags_None) \
	X(NO_AUTO_CLOSE_POPUPS, ImGuiSelectableFlags_NoAutoClosePopups) \
	X(SPAN_ALL_COLUMNS, ImGuiSelectableFlags_SpanAllColumns) \
	X(ALLOW_DOUBLE_CLICK, ImGuiSelectableFlags_AllowDoubleClick) \
	X(DISABLED, ImGuiSelectableFlags_Disabled) \
	X(ALLOW_OVERLAP, ImGuiSelectableFlags_AllowOverlap) \
	X(HIGHLIGHT, ImGuiSelectableFlags_Highlight)

	//! ImGuiComboFlags
#define GUICOLLECTION_FLAG_COMBO \
	X(NONE, ImGuiComboFlags_None) \
	X(POPUP_ALIGN_LEFT, ImGuiComboFlags_PopupAlignLeft) \
	X(HEIGHT_SMALL, ImGuiComboFlags_HeightSmall) \
	X(HEIGHT_REGULAR, ImGuiComboFlags_HeightRegular) \
	X(HEIGHT_LARGE, ImGuiComboFlags_HeightLarge) \
	X(HEIGHT_LARGEST, ImGuiComboFlags_HeightLargest) \
	X(NO_ARROW_BUTTON, ImGuiComboFlags_NoArrowButton) \
	X(NO_PREVIEW, ImGuiComboFlags_NoPreview) \
	X(WIDTH_FIT_PREVIEW, ImGuiComboFlags_WidthFitPreview)

	//! ImGuiCol
#define GUICOLLECTION_FLAG_COLOR \
	X(TEXT, ImGuiCol_Text) \
	X(TEXT_DISABLED, ImGuiCol_TextDisabled) \
	X(WINDOW_BG, ImGuiCol_WindowBg) \
	X(CHILD_BG, ImGuiCol_ChildBg) \
	X(POPUP_BG, ImGuiCol_PopupBg) \
	X(BORDER, ImGuiCol_Border) \
	X(BORDER_SHADOW, ImGuiCol_BorderShadow) \
	X(FRAME_BG, ImGuiCol_FrameBg) \
	X(FRAME_BG_HOVERED, ImGuiCol_FrameBgHovered) \
	X(FRAME_BG_ACTIVE, ImGuiCol_FrameBgActive) \
	X(TITLE_BG, ImGuiCol_TitleBg) \
	X(TITLE_BG_ACTIVE, ImGuiCol_TitleBgActive) \
	X(TITLE_BG_COLLAPSED, ImGuiCol_TitleBgCollapsed) \
	X(MENU_BAR_BG, ImGuiCol_MenuBarBg) \
	X(SCROLL_BAR_BG, ImGuiCol_ScrollbarBg) \
	X(SCROLL_BAR_GRAB, ImGuiCol_ScrollbarGrab) \
	X(SCROLL_BAR_GRAB_HOVERED, ImGuiCol_ScrollbarGrabHovered) \
	X(SCROLL_BAR_GRAB_ACTIVE, ImGuiCol_ScrollbarGrabActive) \
	X(CHECK_MARK, ImGuiCol_CheckMark) \
	X(SLIDER_GRAB, ImGuiCol_SliderGrab) \
	X(SLIDER_GRAB_ACTIVE, ImGuiCol_SliderGrabActive) \
	X(BUTTON, ImGuiCol_Button) \
	X(BUTTON_HOVERED, ImGuiCol_ButtonHovered) \
	X(BUTTON_ACTIVE, ImGuiCol_ButtonActive) \
	X(HEADER, ImGuiCol_Header) \
	X(HEADER_HOVERED, ImGuiCol_HeaderHovered) \
	X(HEADER_ACTIVE, ImGuiCol_HeaderActive) \
	X(SEPARATOR, ImGuiCol_Separator) \
	X(SEPARATOR_HOVERED, ImGuiCol_SeparatorHovered) \
	X(SEPARATOR_ACTIVE, ImGuiCol_SeparatorActive) \
	X(RESIZE_GRIP, ImGuiCol_ResizeGrip) \
	X(RESIZE_GRIP_HOVERED, ImGuiCol_ResizeGripHovered) \
	X(RESIZE_GRIP_ACTIVE, ImGuiCol_ResizeGripActive) \
	X(TAB_HOVERED, ImGuiCol_TabHovered) \
	X(TAB, ImGuiCol_Tab) \
	X(TAB_SELECTED, ImGuiCol_TabSelected) \
	X(TAB_SELECTED_OVERLINE, ImGuiCol_TabSelectedOverline) \
	X(TAB_DIMMED, ImGuiCol_TabDimmed) \
	X(TAB_DIMMED_SELECTED, ImGuiCol_TabDimmedSelected) \
	X(TAB_DIMMED_SELECTED_OVERLINE, ImGuiCol_TabDimmedSelectedOverline) \
	X_DEPEND_DOCKING(DOCKING_PREVIEW, ImGuiCol_DockingPreview) \
	X_DEPEND_DOCKING(DOCKING_EMPTY_BG, ImGuiCol_DockingEmptyBg) \
	X(PLOT_LINES, ImGuiCol_PlotLines) \
	X(PLOT_LINES_HOVERED, ImGuiCol_PlotLinesHovered) \
	X(PLOT_HISTOGRAM, ImGuiCol_PlotHistogram) \
	X(PLOT_HISTOGRAM_HOVERED, ImGuiCol_PlotHistogramHovered) \
	X(TABLE_HEADER_BG, ImGuiCol_TableHeaderBg) \
	X(TABLE_BORDER_STRONG, ImGuiCol_TableBorderStrong) \
	X(TABLE_BORDER_LIGHT, ImGuiCol_TableBorderLight) \
	X(TABLE_ROW_BG_EVEN_ROWS, ImGuiCol_TableRowBg) \
	X(TABLE_ROW_BG_ODD_ROWS, ImGuiCol_TableRowBgAlt) \
	X(TEXT_LINK, ImGuiCol_TextLink) \
	X(TEXT_SELECTED_BG, ImGuiCol_TextSelectedBg) \
	X(DRAG_DROP_TARGET, ImGuiCol_DragDropTarget) \
	X(NAV_HIGHLIGHT, ImGuiCol_NavHighlight) \
	X(NAV_WINDOWING_HIGHLIGHT, ImGuiCol_NavWindowingHighlight) \
	X(NAV_WINDOWING_DIM_BG, ImGuiCol_NavWindowingDimBg) \
	X(MODAL_WINDOW_DIM_BG, ImGuiCol_ModalWindowDimBg)

	//! ImGuiStyleVar
#define GUICOLLECTION_FLAG_STYLE_VAR \
	X(ALPHA, ImGuiStyleVar_Alpha) \
	X(DISABLED_ALPHA, ImGuiStyleVar_DisabledAlpha) \
	X(WINDOW_PADDING, ImGuiStyleVar_WindowPadding) \
	X(WINDOW_ROUNDING, ImGuiStyleVar_WindowRounding) \
	X(WINDOW_BORDER_SIZE, ImGuiStyleVar_WindowBorderSize) \
	X(WINDOW_MIN_SIZE, ImGuiStyleVar_WindowMinSize) \
	X(WINDOW_TITLE_ALIGN, ImGuiStyleVar_WindowTitleAlign) \
	X(CHILD_ROUNDING, ImGuiStyleVar_ChildRounding) \
	X(CHILD_BORDER_SIZE, ImGuiStyleVar_ChildBorderSize) \
	X(POPUP_ROUNDING, ImGuiStyleVar_PopupRounding) \
	X(POPUP_BORDER_SIZE, ImGuiStyleVar_PopupBorderSize) \
	X(FRAME_PADDING, ImGuiStyleVar_FramePadding) \
	X(FRAME_ROUNDING, ImGuiStyleVar_FrameRounding) \
	X(FRAME_BORDER_SIZE, ImGuiStyleVar_FrameBorderSize) \
	X(ITEM_SPACING, ImGuiStyleVar_ItemSpacing) \
	X(ITEM_INNER_SPACING, ImGuiStyleVar_ItemInnerSpacing) \
	X(INDENT_SPACING, ImGuiStyleVar_IndentSpacing) \
	X(CELL_PADDING, ImGuiStyleVar_CellPadding) \
	X(SCROLL_BAR_SIZE, ImGuiStyleVar_ScrollbarSize) \
	X(SCROLL_BAR_ROUNDING, ImGuiStyleVar_ScrollbarRounding) \
	X(GRAB_MIN_SIZE, ImGuiStyleVar_GrabMinSize) \
	X(GRAB_ROUNDING, ImGuiStyleVar_GrabRounding) \
	X(TAB_ROUNDING, ImGuiStyleVar_TabRounding) \
	X(TAB_BORDER_SIZE, ImGuiStyleVar_TabBorderSize) \
	X(TAB_BAR_BORDER_SIZE, ImGuiStyleVar_TabBarBorderSize) \
	X(TAB_BAR_OVERLINE_SIZE, ImGuiStyleVar_TabBarOverlineSize) \
	X(TABLE_ANGLED_HEADERS_ANGLE, ImGuiStyleVar_TableAngledHeadersAngle) \
	X(TABLE_ANGLED_HEADERS_TEXT_ALIGN, ImGuiStyleVar_TableAngledHeadersTextAlign) \
	X(BUTTON_TEXT_ALIGN, ImGuiStyleVar_ButtonTextAlign) \
	X(SELECTABLE_TEXT_ALIGN, ImGuiStyleVar_SelectableTextAlign) \
	X(SEPARATOR_TEXT_BORDER_SIZE, ImGuiStyleVar_SeparatorTextBorderSize) \
	X(SEPARATOR_TEXT_ALIGN, ImGuiStyleVar_SeparatorTextAlign) \
	X(SEPARATOR_TEXT_TEXT_PADDING, ImGuiStyleVar_SeparatorTextPadding) \
	X_DEPEND_DOCKING(DOCKING_SEPARATOR_SIZE, ImGuiStyleVar_DockingSeparatorSize)

	//! ImGuiColorEditFlags
#define GUICOLLECTION_FLAG_COLOR_EDIT \
	X(NONE, ImGuiColorEditFlags_None) \
	X(NO_ALPHA, ImGuiColorEditFlags_NoAlpha) \
	X(NO_PICKER, ImGuiColorEditFlags_NoPicker) \
	X(NO_OPTIONS, ImGuiColorEditFlags_NoOptions) \
	X(NO_SMALL_PREVIEW, ImGuiColorEditFlags_NoSmallPreview) \
	X(NO_INPUTS, ImGuiColorEditFlags_NoInputs) \
	X(NO_TOOLTIP, ImGuiColorEditFlags_NoTooltip) \
	X(NO_LABEL, ImGuiColorEditFlags_NoLabel) \
	X(NO_SIDE_PREVIEW, ImGuiColorEditFlags_NoSidePreview) \
	X(NO_DRAG_DROP, ImGuiColorEditFlags_NoDragDrop) \
	X(NO_BORDER, ImGuiColorEditFlags_NoBorder) \
	X(ALPHA_PREVIEW_HALF, ImGuiColorEditFlags_AlphaPreviewHalf) \
	X(ALPHA_BAR, ImGuiColorEditFlags_AlphaBar) \
	X(DISPLAY_HDR, ImGuiColorEditFlags_HDR) \
	X(DISPLAY_RGB, ImGuiColorEditFlags_DisplayRGB) \
	X(DISPLAY_HSV, ImGuiColorEditFlags_DisplayHSV) \
	X(DISPLAY_HEX, ImGuiColorEditFlags_DisplayHex) \
	X(UINT8, ImGuiColorEditFlags_Uint8) \
	X(FLOAT, ImGuiColorEditFlags_Float) \
	X(PICKER_HUE_BAR, ImGuiColorEditFlags_PickerHueBar) \
	X(PICKER_HUE_WHEEL, ImGuiColorEditFlags_PickerHueWheel) \
	X(INPUT_RGB, ImGuiColorEditFlags_InputRGB) \
	X(INPUT_HSV, ImGuiColorEditFlags_InputHSV)

	//! ImGuiCond
#define GUICOLLECTION_FLAG_COND \
	X(NONE, ImGuiCond_None) \
	X(ONCE, ImGuiCond_Once) \
	X(FIRST_USE_EVER, ImGuiCond_FirstUseEver) \
	X(APPEARING, ImGuiCond_Appearing)

	//! ImGuiTableFlags
#define GUICOLLECTION_FLAG_TABLE \
	X(NONE, ImGuiTableFlags_None) \
	X(RESIZABLE, ImGuiTableFlags_Resizable) \
	X(REORDERABLE, ImGuiTableFlags_Reorderable) \
	X(HIDEABLE, ImGuiTableFlags_Hideable) \
	X(SORTABLE, ImGuiTableFlags_Sortable) \
	X(NO_SAVED_SETTINGS, ImGuiTableFlags_NoSavedSettings) \
	X(CONTEXT_MENU_IN_BODY, ImGuiTableFlags_ContextMenuInBody) \
	X(ROW_BG, ImGuiTableFlags_RowBg) \
	X(BORDERS_INNER_H, ImGuiTableFlags_BordersInnerH) \
	X(BORDERS_OUTER_H, ImGuiTableFlags_BordersOuterH) \
	X(BORDERS_INNER_V, ImGuiTableFlags_BordersInnerV) \
	X(BORDERS_OUTER_V, ImGuiTableFlags_BordersOuterV) \
	X(BORDERS_H, ImGuiTableFlags_BordersH) \
	X(BORDERS_V, ImGuiTableFlags_BordersV) \
	X(BORDERS_INNER, ImGuiTableFlags_BordersInner) \
	X(BORDERS_OUTER, ImGuiTableFlags_BordersOuter) \
	X(BORDERS, ImGuiTableFlags_Borders) \
	X(NO_BORDERS_IN_BODY, ImGuiTableFlags_NoBordersInBody) \
	X(NO_BORDERS_IN_BODY_UNTIL_RESIZE, ImGuiTableFlags_NoBordersInBodyUntilResize) \
	X(SIZING_FIXED_FIT, ImGuiTableFlags_SizingFixedFit) \
	X(SIZING_FIXED_SAME, ImGuiTableFlags_SizingFixedSame) \
	X(SIZING_STRETCH_PROPORTIONAL, ImGuiTableFlags_SizingStretchProp) \
	X(SIZING_STRETCH_SAME, ImGuiTableFlags_SizingStretchSame) \
	X(NO_HOST_EXTEND_X, ImGuiTableFlags_NoHostExtendX) \
	X(NO_HOST_EXTEND_Y, ImGuiTableFlags_NoHostExtendY) \
	X(NO_KEEP_COLUMNS_VISIBLE, ImGuiTableFlags_NoKeepColumnsVisible) \
	X(PRECISE_WIDTHS, ImGuiTableFlags_PreciseWidths) \
	X(NO_CLIP, ImGuiTableFlags_NoClip) \
	X(PAD_OUTER_X, ImGuiTableFlags_PadOuterX) \
	X(NO_PAD_OUTER_X, ImGuiTableFlags_NoPadOuterX) \
	X(NO_PAD_INNER_X, ImGuiTableFlags_NoPadInnerX) \
	X(SCROLL_X, ImGuiTableFlags_ScrollX) \
	X(SORT_MULTI, ImGuiTableFlags_SortMulti) \
	X(SORT_TRISTATE, ImGuiTableFlags_SortTristate) \
	X(HIGHLIGHT_HOVERED_COLUMN, ImGuiTableFlags_HighlightHoveredColumn) \
	X(HIDE_HEADER, ImGuiTableFlags_HighlightHoveredColumn << 1) // Custom flag, disables the row header so you can set column widths without headers.

	//! ImGuiTableColumnFlags
#define GUICOLLECTION_FLAG_TABLE_COLUMN \
	X(NONE, ImGuiTableColumnFlags_None) \
	X(DISABLED, ImGuiTableColumnFlags_Disabled) \
	X(DEFAULT_HIDE, ImGuiTableColumnFlags_DefaultHide) \
	X(DEFAULT_SORT, ImGuiTableColumnFlags_DefaultSort) \
	X(WIDTH_STRETCH, ImGuiTableColumnFlags_WidthStretch) \
	X(WIDTH_FIXED, ImGuiTableColumnFlags_WidthFixed) \
	X(NO_RESIZE, ImGuiTableColumnFlags_NoResize) \
	X(NO_REORDER, ImGuiTableColumnFlags_NoReorder) \
	X(NO_HIDE, ImGuiTableColumnFlags_NoHide) \
	X(NO_CLIP, ImGuiTableColumnFlags_NoClip) \
	X(NO_SORT, ImGuiTableColumnFlags_NoSort) \
	X(NO_SORT_ASCENDING, ImGuiTableColumnFlags_NoSortAscending) \
	X(NO_SORT_DESCENDING, ImGuiTableColumnFlags_NoSortDescending) \
	X(NO_HEADER_LABEL, ImGuiTableColumnFlags_NoHeaderLabel) \
	X(NO_HEADER_WIDTH, ImGuiTableColumnFlags_NoHeaderWidth) \
	X(PREFER_SORT_ASCENDING, ImGuiTableColumnFlags_PreferSortAscending) \
	X(PREFER_SORT_DESCENDING, ImGuiTableColumnFlags_PreferSortDescending) \
	X(ENABLE_INDENT, ImGuiTableColumnFlags_IndentEnable) \
	X(DISABLE_INDENT, ImGuiTableColumnFlags_IndentDisable) \
	X(ANGLED_HEADER, ImGuiTableColumnFlags_AngledHeader) \
	X(IS_ENABLED, ImGuiTableColumnFlags_IsEnabled) \
	X(IS_VISIBLE, ImGuiTableColumnFlags_IsVisible) \
	X(IS_SORTED, ImGuiTableColumnFlags_IsSorted) \
	X(IS_HOVERED, ImGuiTableColumnFlags_IsHovered)

#ifndef IMGUI_DISABLE
#define X(enumName, value) enumName = value,
#ifdef IMGUI_HAS_DOCK
#define X_DEPEND_DOCKING(enumName, value) enumName = value,
#else // IMGUI_HAS_DOCK / !IMGUI_HAS_DOCK
#define X_DEPEND_DOCKING(enumName, value) enumName = 0,
#endif // !IMGUI_HAS_DOCK
#else // IMGUI_ENABLED / !IMGUI_ENABLED
#define X(enumName, value) enumName,
#define X_DEPEND_DOCKING(enumName, value) enumName,
#endif // !IMGUI_ENABLED

	/*****************************************************************//*!
	\enum class KEY
	\brief
		ImGuiKey
	*//******************************************************************/
	enum class KEY : int {
		GUICOLLECTION_KEY
	};
	GENERATE_ENUM_CLASS_ITERATION_OPERATORS(KEY)

	/*****************************************************************//*!
	\enum class MOUSE_BUTTON
	\brief
		ImGuiMouseButton
	*//******************************************************************/
	enum class MOUSE_BUTTON : int {
		GUICOLLECTION_MOUSE_BUTTON
	};
	GENERATE_ENUM_CLASS_ITERATION_OPERATORS(MOUSE_BUTTON)

	/*****************************************************************//*!
	\enum class FLAG_WINDOW
	\brief
		ImGuiWindowFlags
	*//******************************************************************/
	enum class FLAG_WINDOW : int {
		GUICOLLECTION_FLAG_WINDOW
	};
	GENERATE_ENUM_CLASS_BITWISE_OPERATORS(FLAG_WINDOW)

	/*****************************************************************//*!
	\enum class FLAG_CHILD
	\brief
		ImGuiChildFlags
	*//******************************************************************/
	enum class FLAG_CHILD : int {
		GUICOLLECTION_FLAG_CHILD
	};
	GENERATE_ENUM_CLASS_BITWISE_OPERATORS(FLAG_CHILD)

	/*****************************************************************//*!
	\enum class FLAG_INPUT_TEXT
	\brief
		ImGuiInputTextFlags
	*//******************************************************************/
	enum class FLAG_INPUT_TEXT : int {
		GUICOLLECTION_FLAG_INPUT_TEXT
	};
	GENERATE_ENUM_CLASS_BITWISE_OPERATORS(FLAG_INPUT_TEXT)

	/*****************************************************************//*!
	\enum class FLAG_TREE_NODE
	\brief
		ImGuiTreeNodeFlags
	*//******************************************************************/
	enum class FLAG_TREE_NODE : int {
		GUICOLLECTION_FLAG_TREE_NODE
	};
	GENERATE_ENUM_CLASS_BITWISE_OPERATORS(FLAG_TREE_NODE)

	/*****************************************************************//*!
	\enum class FLAG_POPUP
	\brief
		ImGuiPopupFlags
	*//******************************************************************/
	enum class FLAG_POPUP : int {
		GUICOLLECTION_FLAG_POPUP
	};
	GENERATE_ENUM_CLASS_BITWISE_OPERATORS(FLAG_POPUP)

	/*****************************************************************//*!
	\enum class FLAG_SELECTABLE
	\brief
		ImGuiSelectableFlags
	*//******************************************************************/
	enum class FLAG_SELECTABLE : int {
		GUICOLLECTION_FLAG_SELECTABLE
	};
	GENERATE_ENUM_CLASS_BITWISE_OPERATORS(FLAG_SELECTABLE)

	/*****************************************************************//*!
	\enum class FLAG_COMBO
	\brief
		ImGuiComboFlags
	*//******************************************************************/
	enum class FLAG_COMBO : int {
		GUICOLLECTION_FLAG_COMBO
	};
	GENERATE_ENUM_CLASS_BITWISE_OPERATORS(FLAG_COMBO)

	/*****************************************************************//*!
	\enum class FLAG_PAYLOAD_SOURCE
	\brief
		ImGuiDragDropFlags (only those with prefix 'Source')
	*//******************************************************************/
	enum class FLAG_PAYLOAD_SOURCE : int {
		GUICOLLECTION_FLAG_PAYLOAD_SOURCE
	};
	GENERATE_ENUM_CLASS_BITWISE_OPERATORS(FLAG_PAYLOAD_SOURCE)

	/*****************************************************************//*!
	\enum class FLAG_PAYLOAD_TARGET
	\brief
		ImGuiDragDropFlags (only those with prefix 'Accept')
	*//******************************************************************/
	enum class FLAG_PAYLOAD_TARGET : int {
		GUICOLLECTION_FLAG_PAYLOAD_TARGET
	};
	GENERATE_ENUM_CLASS_BITWISE_OPERATORS(FLAG_PAYLOAD_TARGET)

	/*****************************************************************//*!
	\enum class FLAG_STYLE_COLOR
	\brief
		ImGuiCol
	*//******************************************************************/
	enum class FLAG_STYLE_COLOR : int {
		GUICOLLECTION_FLAG_COLOR
	};
	// No bitwise operators for this flag as they aren't supposed to be combined (these aren't binary flags)

	/*****************************************************************//*!
	\enum class FLAG_STYLE_VAR
	\brief
		ImGuiStyleVar
	*//******************************************************************/
	enum class FLAG_STYLE_VAR : int {
		GUICOLLECTION_FLAG_STYLE_VAR
	};
	// No bitwise operators for this flag as they aren't supposed to be combined (these aren't binary flags)

	/*****************************************************************//*!
	\enum class FLAG_COLOR_EDIT
	\brief
		ImGuiColorEditFlags
	*//******************************************************************/
	enum class FLAG_COLOR_EDIT : int {
		GUICOLLECTION_FLAG_COLOR_EDIT
	};
	GENERATE_ENUM_CLASS_BITWISE_OPERATORS(FLAG_COLOR_EDIT)

	/*****************************************************************//*!
	\enum class FLAG_COND
	\brief
		ImGuiCond
	*//******************************************************************/
	enum class FLAG_COND : int {
		GUICOLLECTION_FLAG_COND
	};
	// No bitwise operators for this flag as they aren't supposed to be combined (see ImGuiCond_ comment)

	/*****************************************************************//*!
	\enum class FLAG_TABLE
	\brief
		ImGuiTableFlags
	*//******************************************************************/
	enum class FLAG_TABLE : int {
		GUICOLLECTION_FLAG_TABLE
	};
	GENERATE_ENUM_CLASS_BITWISE_OPERATORS(FLAG_TABLE)

	/*****************************************************************//*!
	\enum class FLAG_TABLE_COLUMN
	\brief
		ImGuiTableColumnFlags
	*//******************************************************************/
	enum class FLAG_TABLE_COLUMN : int {
		GUICOLLECTION_FLAG_TABLE_COLUMN
	};
	GENERATE_ENUM_CLASS_BITWISE_OPERATORS(FLAG_TABLE_COLUMN)

#pragma endregion // Flags

}

namespace gui {
	namespace internal {

		/*****************************************************************//*!
		\class BeginEndBound
		\brief
			A helper base class automatically providing the logic to call specified
			Begin() and End() functions based on the object's lifetime, so us programmers
			will never forget to call the 2 functions as a pair at the correct timing.
		\tparam StartFunc
			The function called when the object is created.
		\tparam EndFunc
			The function called when the object is destroyed only if StartFunc returned true.
			If nullptr, nothing happens.
		\tparam AlwaysCallEnd
			Whether to always call EndFunc() even if StartFunc() returned false.
		\tparam StartIsOptional
			Whether the first argument passed to args is a bool indicating whether to call StartFunc() or not.
		*//******************************************************************/
		template <auto StartFunc, void (*EndFunc)() = nullptr, bool AlwaysCallEnd = false, bool StartIsOptional = false>
		class BeginEndBound
		{
		public:
			/*****************************************************************//*!
			\brief
				Constructor. Calls StartFunc().
				// Pass by value because most arguments in ImGui are small.
			\tparam Args
				The types of the arguments.
			\tparam args
				The arguments to StartFunc(). If StartIsOptional is true, the first argument
				must be a bool that determines whether to call StartFunc().
			*//******************************************************************/
			template <typename ...Args>
			BeginEndBound(Args... args);

			/*****************************************************************//*!
			\brief
				Destructor.
			*//******************************************************************/
			~BeginEndBound();

			/*****************************************************************//*!
			\brief
				Returns whether StartFunc() succeeded.
			\return
				True if StartFunc() returned true or no value. False if StartFunc() returned false.
			*//******************************************************************/
			operator bool() const;

		protected:
			bool GetIsOpen();

		private:
			/*****************************************************************//*!
			\brief
				Calls StartFunc() with the provided arguments, if StartFunc() exists.
				// Pass by value because most arguments in ImGui are small.
			\tparam Args
				The types of the arguments.
			\tparam args
				The arguments to StartFunc().
			\return
				The return value of StartFunc(). True if StartFunc() doesn't return a bool.
			*//******************************************************************/
			template <typename ...Args>
			static bool CallStartFunc(Args... args);

			/*****************************************************************//*!
			\brief
				Calls StartFunc() with the provided arguments depending on the template's parameters and provided arguments.
				// Pass by value because most arguments in ImGui are small.
			\tparam Args
				The types of the arguments.
			\tparam args
				The arguments to StartFunc(). If StartIsOptional is true, the first argument must be a bool.
			\return
				The return value of StartFunc(). True if StartFunc() doesn't return a bool.
				If StartIsOptional is true and the first argument is false, returns false.
			*//******************************************************************/
			template <typename ...Args>
			static bool CallStartFuncBasedOnParams(Args... args);

		private:
			//! The value that StartFunc() returned.
			bool isOpen;
		};

		// For disambiguation
#ifndef IMGUI_DISABLE
		//! ImGui::BeginChild()
		bool BeginChild(const char* str_id, const ImVec2& size, ImGuiChildFlags child_flags, ImGuiWindowFlags window_flags);

		//! ImGui::TreeNodeEx
		bool TreeNodeEx(const char* label, ImGuiTreeNodeFlags flags);
		//! ImGui::CollapsingHeader()
		bool CollapsingHeader(const char* label, ImGuiTreeNodeFlags flags);

		//! ImGui::PushStyleVar
		void PushStyleVar(ImGuiStyleVar idx, const ImVec2& val);
		//! ImGui::PopStyleVar (remove arguments)
		void PopStyleVar();

		//! ImGui::PushStyleColor
		void PushStyleColor(ImGuiCol idx, const ImVec4& val);
		//! ImGui::PopStyleColor (remove arguments)
		void PopStyleColor();
#endif

#ifndef IMGUI_DISABLE
		using BeginEndBound_Child = BeginEndBound<BeginChild, ImGui::EndChild, true>;
		using BeginEndBound_Popup = BeginEndBound<ImGui::BeginPopup, ImGui::EndPopup>;
		using BeginEndBound_Tooltip = BeginEndBound<ImGui::BeginTooltip, ImGui::EndTooltip, false, true>;

		using BeginEndBound_TextWrapPos = BeginEndBound<ImGui::PushTextWrapPos, ImGui::PopTextWrapPos>;

		using BeginEndBound_PayloadSource = BeginEndBound<ImGui::BeginDragDropSource, ImGui::EndDragDropSource>;

		using BeginEndBound_MainMenuBar = BeginEndBound<ImGui::BeginMainMenuBar, ImGui::EndMainMenuBar>;
		using BeginEndBound_MenuBar = BeginEndBound<ImGui::BeginMenuBar, ImGui::EndMenuBar>;
		using BeginEndBound_Menu = BeginEndBound<ImGui::BeginMenu, ImGui::EndMenu>;
		using BeginEndBound_ItemContextMenu = BeginEndBound<ImGui::BeginPopupContextItem, ImGui::EndPopup>;

		using BeginEndBound_TreeNode = BeginEndBound<TreeNodeEx, ImGui::TreePop>;
		using BeginEndBound_CollapsingHeader = BeginEndBound<CollapsingHeader>;

		using BeginEndBound_Table = BeginEndBound<ImGui::BeginTable, ImGui::EndTable>;

		using BeginEndBound_Group = BeginEndBound<ImGui::BeginGroup, ImGui::EndGroup>;

		using BeginEndBound_StyleVar = BeginEndBound<PushStyleVar, PopStyleVar>;
		using BeginEndBound_StyleColor = BeginEndBound<PushStyleColor, PopStyleColor, false, true>;

		using BeginEndBound_Disabled = BeginEndBound<ImGui::BeginDisabled, ImGui::EndDisabled>;
#else
		using BeginEndBound_Child = std::false_type;
		using BeginEndBound_Popup = std::false_type;
		using BeginEndBound_Tooltip = std::false_type;

		using BeginEndBound_TextWrapPos = std::false_type;

		using BeginEndBound_PayloadSource = std::false_type;

		using BeginEndBound_MainMenuBar = std::false_type;
		using BeginEndBound_MenuBar = std::false_type;
		using BeginEndBound_Menu = std::false_type;
		using BeginEndBound_ItemContextMenu = std::false_type;

		using BeginEndBound_TreeNode = std::false_type;
		using BeginEndBound_CollapsingHeader = std::false_type;

		using BeginEndBound_Table = std::false_type;

		using BeginEndBound_Group = std::false_type;

		using BeginEndBound_StyleVar = std::false_type;
		using BeginEndBound_StyleColor = std::false_type;

		using BeginEndBound_Disabled = std::false_type;
#endif

		/*****************************************************************//*!
		\class ContainerBase
		\brief
			A base that describes a generic window that contains content.
		*//******************************************************************/
		class ContainerBase
		{
		public:
			/*****************************************************************//*!
			\brief
				Constructor.
			\param title
				The title of the container.
			\param initialDimensions
				The dimensions of the window
			\param windowSizeCondFlags
				The flags of the setting of the window size
			*//******************************************************************/
			ContainerBase(TextTypeExtra title, const Vec2& initialDimensions, FLAG_COND windowSizeCondFlags = FLAG_COND::NONE);

			/*****************************************************************//*!
			\brief
				Draws the container depending on whether the container is shown.
			\param id
				To help ImGui tell identical windows apart. Does nothing if 0 or negative.
			*//******************************************************************/
			void Draw(int id = 0);

		protected:
			/*****************************************************************//*!
			\brief
				Called when the container is drawn. This function should call ImGui functions
				that set up and tear down the window.
			\param id
				To help ImGui tell identical windows apart. Does nothing if 0 or negative.
			*//******************************************************************/
			virtual void DrawContainer(int id) = 0;

			/*****************************************************************//*!
			\brief
				Called when the window is drawn. Custom implementations should override
				this and call various other gui namespace to populate the ImGui window
				with content.
			*//******************************************************************/
			virtual void DrawContents() = 0;

		protected:
			//! Whether this container is open
			bool isOpen;
			//! The title of this container
			std::string title;

		private:
			//! The initial size of this container
			Vec2 initialDimensions;
			//! The conditions that are passed to the ImGui function setting the container's dimensions.
			FLAG_COND windowSizeCondFlags;

		};

		/*****************************************************************//*!
		\class TextType
		\brief
			Helper type to combine const char* and std::string to deduplicate
			functions that need to support both.
		*//******************************************************************/
		struct TextType
		{
			TextType(const char* text);
			TextType(const std::string_view& text);
			TextType(const std::string& text);
			operator const char* () const;

			const char* data() const;
			size_t size() const;
			bool empty() const;

			const char* text;
		};
		/*****************************************************************//*!
		\class TextTypeExtra
		\brief
			Same as above but doubles the size to maintain information about
			the string via std::string_view.
		*//******************************************************************/
		struct TextTypeExtra : public std::string_view
		{
			TextTypeExtra(const char* text);
			TextTypeExtra(const std::string_view& text);
			TextTypeExtra(const std::string& text);
			operator const char* () const;
		};

#ifdef GUI_FEATURE_CPP20_FORMAT
		// Applies std::vformat() to the TextType struct.
		// To resolve disambiguation issues with std::vformat() and TextType.
		template <typename... Args>
		std::string format(TextTypeExtra text, const Args&... args);

		// Implementations using format() will prefer to have std::string_view data.
		using TextTypeForFormatting = TextTypeExtra;
#else
		// Implementations using printf() don't need extra information.
		using TextTypeForFormatting = TextType;
#endif

}
}

namespace gui {

#pragma region Containers

	/*****************************************************************//*!
	\class Window
	\brief
		Wraps ImGui::Begin() and ImGui::End(). Draws an ImGui window.
	*//******************************************************************/
	class Window : public internal::ContainerBase
	{
	protected:
		/*****************************************************************//*!
		\brief
			Constructor.
		\param title
			The name of the window.
		\param initDimensions
			The initial width and height of the window.
		\param windowFlags
			The flags of the window.
		*//******************************************************************/
		Window(internal::TextTypeExtra title, const Vec2& initDimensions, FLAG_WINDOW windowFlags = FLAG_WINDOW::NONE);

		/*****************************************************************//*!
		\brief
			Draws this window. If overriding to modify some window style etc,
			remember to call base as otherwise nothing will draw!
		\param id
			To help ImGui tell identical windows apart. Does nothing if 0 or negative.
		*//******************************************************************/
		virtual void DrawContainer(int id) override;

	public:
		/*****************************************************************//*!
		\brief
			Gets whether this window is open.
		\return
			True if the window is open. False otherwise.
		*//******************************************************************/
		bool GetIsOpen() const;

		/*****************************************************************//*!
		\brief
			Sets whether this window is open.
		\param
			True if the window is open. False otherwise.
		*//******************************************************************/
		void SetIsOpen(bool newIsOpen);

	protected:
		/*****************************************************************//*!
		\brief
			Called when the state of isOpen changes. Implementations may hook onto
			this for special logic that executes when isOpen changes.
		*//******************************************************************/
		virtual void OnOpenStateChanged();

	protected:
		//! The window flags
		FLAG_WINDOW windowFlags;

	};

	/*****************************************************************//*!
	\class Child
	\brief
		Wraps ImGui::BeginChild() and ImGui::EndChild().
	*//******************************************************************/
	class Child : public internal::BeginEndBound_Child
	{
	public:
		//! ImGui::BeginChild()
		Child(internal::TextType str_id, const Vec2& size = Vec2{}, FLAG_CHILD child_flags = FLAG_CHILD::NONE, FLAG_WINDOW window_flags = FLAG_WINDOW::NONE);
	};

	/*****************************************************************//*!
	\class Group
	\brief
		Wraps ImGui::BeginGroup() and ImGui::EndGroup().
	*//******************************************************************/
	class Group : public internal::BeginEndBound_Group
	{
	};

	/*****************************************************************//*!
	\class Popup
	\brief
		Wraps ImGui::BeginPopup() and ImGui::EndPopup().
	*//******************************************************************/
	class Popup : public internal::BeginEndBound_Popup
	{
	public:
		//! ImGui::BeginPopup()
		Popup(internal::TextType identifier, FLAG_WINDOW flags = FLAG_WINDOW::NONE);

		/*****************************************************************//*!
		\brief
			Sets a popup open state to open.
		\param identifier
			The popup identifier.
		*//******************************************************************/
		static void Open(internal::TextType identifier);

		//! ImGui::CloseCurrentPopup()
		void Close();

		//! ImGui::IsWindowAppearing() - whether this window was opened this frame.
		bool WasOpenedThisFrame();
	};

	/*****************************************************************//*!
	\class PopupWindow
	\brief
		A construct not found in ImGui that wraps boilerplate code that draws an
		ImGui popup window. Derive this class and implement DrawContents() to create
		your own custom ImGui popup window.
	*//******************************************************************/
	class PopupWindow : public internal::ContainerBase
	{
	public:
		/*****************************************************************//*!
		\enum FLAG
		\brief
			Flags specific for popup.
		*//******************************************************************/
		enum class FLAG
		{
			NONE = 0,
			NO_CLOSE_BUTTON = 1,
		};

	protected:
		/*****************************************************************//*!
		\brief
			Constructor.
		\param title
			The name of the popup window.
		\param initDimensions
			The initial width and height of the popup window.
		*//******************************************************************/
		PopupWindow(internal::TextTypeExtra title, const Vec2& initDimensions);

		/*****************************************************************//*!
		\brief
			Draws this popup window. If overriding to modify some window style etc,
			remember to call base as otherwise nothing will draw!
		\param id
			To help ImGui tell identical windows apart. Does nothing if 0 or negative.
		*//******************************************************************/
		virtual void DrawContainer(int id) override;

	private:
		/*****************************************************************//*!
		\brief
			Helper function to DrawContainer(), so we don't make a local string
			if we don't have to.
		\param effectiveTitle
			The title of the popup.
		*//******************************************************************/
		void DrawContainer_Popup(internal::TextType effectiveTitle);

	protected:
		//! The flags of the popup.
		FLAG flags;
	};
	GENERATE_ENUM_CLASS_BITWISE_OPERATORS(PopupWindow::FLAG);

	/*****************************************************************//*!
	\class Tooltip
	\brief
		Wraps ImGui::BeginTooltip() and ImGui::EndTooltip(), or ImGui::SetTooltip().
	*//******************************************************************/
	class Tooltip : public internal::BeginEndBound_Tooltip
	{
	public:
		//! ImGui::BeginTooltip() and ImGui::EndTooltip().
		Tooltip();

		//! ImGui::SetTooltip()
		template <typename ...Args>
		Tooltip(internal::TextTypeForFormatting fmt, const Args&... args);
	};

	//! ImGui::GetWindowWidth
	float GetWindowWidth();
	//! ImGui::GetWindowHeight
	float GetWindowHeight();
	//! ImGui::GetWindowContentRegionMax()
	Vec2 GetWindowContentRegionMax();

	//! ImGui::SetNextWindowPos()
	void SetNextWindowPos(const Vec2& pos, gui::FLAG_COND flags = gui::FLAG_COND::NONE, gui::Vec2 pivot = gui::Vec2{});
	//! ImGui::SetNextWindowSize()
	void SetNextWindowSize(const Vec2& size, gui::FLAG_COND flags = gui::FLAG_COND::NONE);
	//! ImGui::SetNextWindowSizeConstraints
	void SetNextWindowSizeConstraints(const Vec2& min, const Vec2& max);
	//! ImGui::SetNextWindowBgAlpha()
	void SetNextWindowBackgroundAlpha(float alpha);

	//! ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow())
	void BringCurrWindowToFront();

	//! ImGui::GetFrameHeightWithSpacing()
	float GetFrameHeightWithSpacing();

	//! Non-ImGui function. Gets whether the current window is focused.
	bool IsCurrWindowFocused();

	//! ImGui::SetItemDefaultFocus()
	void SetItemDefaultFocus();
	//! ImGui::IsItemFocused()
	bool IsItemFocused();
	//! ImGui::IsItemHovered()
	bool IsItemHovered();

#pragma endregion // Containers

#pragma region ID Management

	/*****************************************************************//*!
	\class SetID
	\brief
		Wraps ImGui::PushID() and ImGui::PopID(). Used to help ImGui differentiate
		items with the same labels.
	*//******************************************************************/
	class SetID
	{
	public:
		//! ImGui::PushID()
		SetID(int id);
		SetID(size_t id);
		SetID(internal::TextType label);

		//! ImGui::PopID()
		~SetID();
	};

	//! Gets the current ID on the ID stack.
	int GetCurrID();

#pragma endregion // ID Management

#pragma region Text

	//! ImGui::GetFontSize()
	float GetFontSize();
	//! ImGui::CalcTextSize()
	gui::Vec2 CalcTextSize(internal::TextTypeExtra text);

	/*****************************************************************//*!
	\class SetTextWrapPos
	\brief
		Wraps ImGui::PushTextWrapPos() and ImGui::PopTextWrapPos().
	*//******************************************************************/
	class SetTextWrapPos : public internal::BeginEndBound_TextWrapPos
	{
	public:
		//! ImGui::PushTextWrapPos()
		SetTextWrapPos(float wrap_local_pos_x = 0.0f);
	};

	//! ImGui::AlignTextToFramePadding()
	void AlignTextToFramePadding();

	//! ImGui::Text()
	template <typename ...Args>
	void Text(internal::TextTypeForFormatting fmt, const Args&... args);
	// Custom function to render text in the center of available area
	template <typename ...Args>
	void TextCentered(internal::TextTypeForFormatting fmt, const Args&... args);
	//! ImGui::TextColored
	template <typename ...Args>
	void TextColored(const Vec4& color, internal::TextTypeForFormatting fmt, const Args&... args);
	//! ImGui::TextWrapped()
	template <typename ...Args>
	void TextWrapped(internal::TextTypeForFormatting fmt, const Args&... args);
	//! ImGui::TextDisabled()
	template <typename ...Args>
	void TextDisabled(internal::TextTypeForFormatting fmt, const Args&... args);

	//! ImGui::InputText()
	bool TextBox(const char* label, char* text, size_t maxSize, FLAG_INPUT_TEXT flags = FLAG_INPUT_TEXT::NONE);
	bool TextBox(const char* label, std::string* text, FLAG_INPUT_TEXT flags = FLAG_INPUT_TEXT::NONE);
	//! ImGui::InputText() (displays read-only text within a text box)
	void TextBoxReadOnly(const char* label, const char* text, size_t size);
	void TextBoxReadOnly(const char* label, internal::TextTypeExtra text);

	//! Custom function to test whether a filter matches the start of a string
	bool PassFilterSimple(internal::TextTypeExtra filterText, internal::TextTypeExtra candidateText);

	/*****************************************************************//*!
	\class AdvancedFilter
	\brief
		Emulates the filter algorithm built into ImGuiTextFilter, based on v1.91.4.
		- Supports multiple terms delimited by ','
		- Ignores leading and trailing whitespace
		- Candidates containing terms that start with '-' are filtered out
		- Candidates that don't contain any positive term are filtered out
	*//******************************************************************/
	class AdvancedFilter
	{
	public:
		/*****************************************************************//*!
		\brief
			Constructor.
		\param filterText
			Filter terms delimited by ','
		*//******************************************************************/
		AdvancedFilter(internal::TextTypeExtra filterText);

		/*****************************************************************//*!
		\brief
			Tests whether a candidate text passes the filter.
		\param candidateText
			The candidate text.
		\return
			True or false depending on the conditions in this class's brief.
		*//******************************************************************/
		bool PassFilter(internal::TextTypeExtra candidateText);

	private:
		std::vector<std::string_view> positiveFilter;
		std::vector<std::string_view> negativeFilter;
	};

#pragma endregion // Text

#pragma region Variables

	//! ImGui::Selectable
	bool Selectable(internal::TextType label, bool isSelected = false, gui::Vec2 size = gui::Vec2{}, FLAG_SELECTABLE flags = FLAG_SELECTABLE::NONE);

	//! ImGui::Checkbox
	bool Checkbox(internal::TextType label, bool* v);

	//! ImGui::DragInt, etc.
	bool VarDrag(internal::TextType label, int* v, float speed = 1.0f, int min = 0, int max = 0);
	bool VarDrag(internal::TextType label, unsigned int* v, float speed = 1.0f, unsigned int min = 0, unsigned int max = std::numeric_limits<int>::max());
	bool VarDrag(internal::TextType label, size_t* v, float speed = 1.0f, size_t min = 0, size_t max = std::numeric_limits<size_t>::max());
	bool VarDrag(internal::TextType label, float* v, float speed = 1.0f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f");
	bool VarDrag(internal::TextType label, double* v, double speed = 1.0f, double min = 0.0f, double max = 0.0f, const char* format = "%.3f");
#ifdef GUI_FEATURE_CUSTOM_MATH_VEC_CLASSES
	bool VarDrag(internal::TextTypeForFormatting label, internal::ProjectVec2* v, float speed = 1.0f, internal::ProjectVec2 min = {}, internal::ProjectVec2 max = {}, const char* format = "%.2f");
	bool VarDrag(internal::TextTypeForFormatting label, internal::ProjectVec3* v, float speed = 1.0f, internal::ProjectVec3 min = {}, internal::ProjectVec3 max = {}, const char* format = "%.2f");
	bool VarDrag(internal::TextTypeForFormatting label, internal::ProjectVec4* v, float speed = 1.0f, internal::ProjectVec4 min = {}, internal::ProjectVec4 max = {}, const char* format = "%.1f");
#endif // GUI_FEATURE_CUSTOM_MATH_VEC_CLASSES

	//! ImGui::InputInt, etc.
	bool VarInput(internal::TextType label, int* v, int step = 1);
	bool VarInput(internal::TextType label, unsigned int* v, unsigned int step = 1);
	bool VarInput(internal::TextType label, size_t* v, size_t step = 1);
	bool VarInput(internal::TextType label, float* v, float step = 1.0f);
	bool VarInput(internal::TextType label, double* v, double step = 1.0);
#ifdef GUI_FEATURE_CUSTOM_MATH_VEC_CLASSES
	bool VarInput(internal::TextType label, internal::ProjectVec2* v);
	bool VarInput(internal::TextType label, internal::ProjectVec3* v);
	bool VarInput(internal::TextType label, internal::ProjectVec4* v);
#endif // GUI_FEATURE_CUSTOM_MATH_VEC_CLASSES

	//! ImGui::ColorEdit
#ifdef GUI_FEATURE_CUSTOM_MATH_VEC_CLASSES
	bool VarColor(internal::TextType label, internal::ProjectVec3* v, FLAG_COLOR_EDIT flags = FLAG_COLOR_EDIT::NONE);
	bool VarColor(internal::TextType label, internal::ProjectVec4* v, FLAG_COLOR_EDIT flags = FLAG_COLOR_EDIT::NONE);
#endif // GUI_FEATURE_CUSTOM_MATH_VEC_CLASSES
#ifndef IMGUI_DISABLE // The below overload is specifically for ImGui's vec version. For ImGui disabled scenarios, this would cause a re-definition of the above function.
	bool VarColor(internal::TextType label, gui::Vec4* v, FLAG_COLOR_EDIT flags = FLAG_COLOR_EDIT::NONE);
#endif // IMGUI_DISABLE

	//! ImGui::SliderInt, etc.
	bool VarSlider(internal::TextType label, int* v, int min, int max);
	bool VarSlider(internal::TextType label, float* v, float min, float max);

	/*****************************************************************//*!
	\brief
		Assigns one of the above functions as the default function used to
		render a variable type.
	\param label
		The field label.
	\param v
		A pointer to the value.
	*//******************************************************************/
	bool VarDefault(internal::TextType label, bool* v);
	bool VarDefault(internal::TextType label, char* v);
	bool VarDefault(internal::TextType label, int* v);
	bool VarDefault(internal::TextType label, unsigned int* v);
	bool VarDefault(internal::TextType label, size_t* v);
	bool VarDefault(internal::TextType label, float* v);
	bool VarDefault(internal::TextType label, double* v);
#ifdef GUI_FEATURE_CUSTOM_MATH_VEC_CLASSES
	bool VarDefault(internal::TextTypeForFormatting label, internal::ProjectVec2* v);
	bool VarDefault(internal::TextTypeForFormatting label, internal::ProjectVec3* v);
	bool VarDefault(internal::TextTypeForFormatting label, internal::ProjectVec4* v);
#endif // GUI_FEATURE_CUSTOM_MATH_VEC_CLASSES
	bool VarDefault(internal::TextType label, std::string* v);

	/*****************************************************************//*!
	\brief
		Custom: Draw a vector with resize options.
	\tparam T
		The type of data stored within the vector.
	\tparam ElemDrawFuncType
		The function type to call to draw each element. Must be able to receive
		params [T].
	\param label
		The vector field label.
	\param v
		The vector.
	\param elemDrawFunc
		The function that draws each element within the vector.
	*//******************************************************************/
	template <typename T, typename ElemDrawFuncType>
	bool VarContainer(internal::TextTypeForFormatting label, std::vector<T>* v, ElemDrawFuncType elemDrawFunc);
	
	// Stores and gets variables in/from ImGui state storage.
	bool& GetVar(internal::TextType label, bool defaultVal);
	int& GetVar(internal::TextType label, int defaultVal);
	float& GetVar(internal::TextType label, float defaultVal);

#pragma endregion // Variables

#pragma region Payload

	/*****************************************************************//*!
	\brief
		Wraps ImGui::BeginDragDropSource(), ImGui::SetDragDropPayload() and ImGui::EndDragDropSource().
		You can draw gui elements to the tooltip box that appears while the user
		drags an item as long as the PayloadSource object is still alive.
	\tparam DataType
		The type of the payload data.
	\param identifier
		The identifier of the payload (to identify compatible payloads)
	\param data
		The payload data.
	\param dragLabel
		The name of the object being dragged. If empty, the tooltip while
		dragging will be empty without any additional GUI calls to draw to it.
	*//******************************************************************/
	template <typename DataType>
	class PayloadSource : public internal::BeginEndBound_PayloadSource
	{
	public:
		PayloadSource(internal::TextType identifier, const DataType& data, FLAG_PAYLOAD_SOURCE flags = FLAG_PAYLOAD_SOURCE::NONE);

	private:
		void SetPayloadTarget(internal::TextType identifier, const DataType& data);
	};
	template<>
	void PayloadSource<std::string>::SetPayloadTarget(internal::TextType identifier, const std::string& data);

	/*****************************************************************//*!
	\brief
		Wraps ImGui::BeginDragDropTarget(), ImGui::AcceptDragDropPayload() and ImGui::EndDragDropTarget().
		Specifying the template parameter "DataType" is required for this function to know what to cast the payload data to.
	\tparam DataType
		The type of the expected payload data.
	\tparam FunctionType
		The type of the function to be called when receiving a payload.
	\param identifier
		The identifier of the payload (to identify compatible payloads)
	\param onReceive
		The function called when receiving a payload.
	*//******************************************************************/
	template <typename DataType, typename FunctionType>
		requires std::invocable<FunctionType, const DataType&>
	void PayloadTarget(internal::TextType identifier, FunctionType onReceive, FLAG_PAYLOAD_TARGET flags = FLAG_PAYLOAD_TARGET::NONE);

	/*****************************************************************//*!
	\brief
		Custom construct that draws an invisible payload target area with
		text in the middle.
	\tparam DataType
		The type of the expected payload data.
	\tparam FunctionType
		The type of the function to be called when receiving a payload.
	\param payloadIdentifier
		The identifier of the payload (to identify compatible payloads)
	\param displayText
		The text in the middle. If nullptr, no text is drawn.
	\param size
		The size of the area.
	\param onReceive
		The function called when receiving a payload.
	*//******************************************************************/
	template <typename DataType, typename FunctionType>
	void PayloadTargetRect(internal::TextType payloadIdentifier, Vec2 size, internal::TextTypeExtra displayText, FunctionType onReceive, FLAG_PAYLOAD_TARGET flags = FLAG_PAYLOAD_TARGET::NONE);

#pragma endregion // Payload

#pragma region Button

	//! ImGui::Button()
	bool Button(internal::TextType label, const Vec2& size = Vec2{});
	//! ImGui::SmallButton()
	bool SmallButton(internal::TextType label);
	//! ImGui::ImageButton()
	bool ImageButton(internal::TextType label, TextureID textureID, Vec2 size, Vec2 uv0 = Vec2{}, Vec2 uv1 = Vec2{ 1.0f, 1.0f });
	//! ImGui::InvisibleButton()
	bool InvisibleButton(internal::TextType label, Vec2 size);

#pragma endregion // Button

#pragma region Combo

	/*****************************************************************//*!
	\class Combo
	\brief
		Wraps ImGui::BeginCombo() and ImGui::EndCombo(), or ImGui::Combo().
	*//******************************************************************/
	class Combo
	{
	public:
		//! ImGui::BeginCombo()
		Combo(internal::TextType label, internal::TextType previewValue, FLAG_COMBO flags = FLAG_COMBO::NONE);

		/*****************************************************************//*!
		\brief
			Calls ImGui::Combo() from a const char* array.
		\tparam ContType
			The type of the array.
		\param label
			The label of the combo.
		\param data
			The array.
		\param selectedValue
			The currently selected string.
		\param outSelectedIndex
			The index of the selected string will be written here.
		*//******************************************************************/
		template <typename ContType>
			requires util::ConvertibleToCArray<ContType> && std::is_same_v<typename ContType::value_type, const char*>
		Combo(internal::TextType label, const ContType& data, internal::TextType selectedValue, int* outSelectedIndex);

		/*****************************************************************//*!
		\brief
			Calls ImGui::Combo() from a const char* array.
		\tparam ContType
			The type of the array.
		\param label
			The label of the combo.
		\param data
			The array.
		\param selectedIndex
			The currently selected index within the array. If a selection
			happens, the new index will be written here.
		*//******************************************************************/
		template <typename ContType>
			requires util::ConvertibleToCArray<ContType> && std::is_same_v<typename ContType::value_type, const char*>
		Combo(internal::TextType label, const ContType& data, int* selectedIndex);

		/*****************************************************************//*!
		\brief
			Calls ImGui::Combo() from a const char* array.
		\tparam ContType
			The type of the array.
		\param label
			The label of the combo.
		\param data
			The array.
		\param selectedIndex
			The currently selected index within the array. If a selection
			happens, the new index will be written here.
		*//******************************************************************/
		Combo(internal::TextType label, const char* const* data, size_t numElems, int* selectedIndex);

		/*****************************************************************//*!
		\brief
			Destructor.
		*//******************************************************************/
		~Combo();

		/*****************************************************************//*!
		\brief
			Adds a selectable entry within this combo. Equivalent to ImGui::Selectable()
		\param label
			The entry's name.
		\param isSelected
			Whether this entry is currently selected.
		\return
			True if this entry was selected this frame update. False otherwise.
		*//******************************************************************/
		bool Selectable(internal::TextType label, bool isSelected);

		/*****************************************************************//*!
		\brief
			NOTE: The behavior of this operator depends on which constructor was called!
			If constructed without elements, returns whether the combo window is open.
			If constructed with elements, returns whether any elements were selected.
		\return
			True or false depending on the conditions listed above.
		*//******************************************************************/
		operator bool() const;

	private:
		/*****************************************************************//*!
		\brief
			Calls ImGui::Combo().
		\param label
			The label of the combo.
		\param data
			Pointer to array of char strings that will be the data that populates the combo.
		\param numElems
			The number of elements within the array.
		\param selectedIndex
			Pointer to the selected index within the array. This will be overriden if a selection happened.
		\return
			True if a selection happened. False otherwise.
		*//******************************************************************/
		void CallCombo(internal::TextType label, const char* const* data, size_t numElems, int* selectedIndex);

	private:
		//! Whether ImGui::EndCombo() should be called. This is set depending on which constructor was called.
		bool shouldCallEndCombo;
		//! Whether the user selected an option this frame update.
		bool selectionHappened{ false };
	};

#pragma endregion // Combo

#pragma region Menu

	/*****************************************************************//*!
	\class MainMenuBar
	\brief
		Wraps ImGui::BeginMainMenuBar() and ImGui::EndMainMenuBar().
	*//******************************************************************/
	class MainMenuBar : public internal::BeginEndBound_MenuBar
	{
	};

	/*****************************************************************//*!
	\class MenuBar
	\brief
		Wraps ImGui::BeginMenuBar() and ImGui::EndMenuBar().
	*//******************************************************************/
	class MenuBar : public internal::BeginEndBound_MenuBar
	{
	};

	/*****************************************************************//*!
	\class Menu
	\brief
		Wraps ImGui::BeginMenu() and ImGui::EndMenu().
	*//******************************************************************/
	class Menu : public internal::BeginEndBound_Menu
	{
	public:
		//! ImGui::BeginMenu()
		Menu(internal::TextType label);

		//! ImGui::MenuItem()
		bool Item(internal::TextType label, bool* p_selected = nullptr);
	};

	/*****************************************************************//*!
	\class ItemContextMenu
	\brief
		Wraps ImGui::BeginPopupContextItem() and ImGui::EndPopup().
		Used for dropdown selection menus when right clicking an item.
	*//******************************************************************/
	class ItemContextMenu : public internal::BeginEndBound_ItemContextMenu
	{
	public:
		//! ImGui::BeginPopupContextItem()
		ItemContextMenu(internal::TextType label, FLAG_POPUP flags = FLAG_POPUP::MOUSE_RIGHT);

		//! ImGui::MenuItem()
		bool Item(internal::TextType label, bool* p_selected = nullptr);
	};

	//! ImGui::MenuItem()
	bool MenuItem(internal::TextType label, bool* p_selected = nullptr);

#pragma endregion // Menu

#pragma region Tree

	/*****************************************************************//*!
	\class TreeNode
	\brief
		Wraps ImGui::TreeNodeEx() and ImGui::TreePop().
	*//******************************************************************/
	class TreeNode : public internal::BeginEndBound_TreeNode
	{
	public:
		//! ImGui::TreeNodeEx()
		TreeNode(internal::TextType label, bool isLeaf, FLAG_TREE_NODE flags = FLAG_TREE_NODE::NONE);
	};

	/*****************************************************************//*!
	\class CollapsingHeader
	\brief
		Wraps ImGui::CollapsingHeader(). Draws a header that is user-collapsable.
	*//******************************************************************/
	class CollapsingHeader : public internal::BeginEndBound_CollapsingHeader
	{
	public:
		//! ImGui::CollapsingHeader()
		CollapsingHeader(internal::TextType label, FLAG_TREE_NODE flags = FLAG_TREE_NODE::NONE);
	};

#pragma endregion // Tree

#pragma region Table

	/*****************************************************************//*!
	\class Table
	\brief
		Wraps ImGui::BeginTable() and ImGui::EndTable().
	*//******************************************************************/
	class Table : public internal::BeginEndBound_Table
	{
	public:
		//! ImGui::BeginTable()
		Table(internal::TextType label, int numCols, bool hasHeader, FLAG_TABLE flags = FLAG_TABLE::NONE, const Vec2& outerSize = Vec2{}, float innerWidth = 0.0f);

		//! ImGui::TableSetupColumn()
		//! This is used to specify each column's properties and header label.
		void AddColumnHeader(internal::TextType label, FLAG_TABLE_COLUMN flags = FLAG_TABLE_COLUMN::NONE, float width = 0.0f);
		//! ImGui::TableHeadersRow()
		//! Submits the column properties to the table. Must be called before populating the first column if hasHeader is true.
		void SubmitColumnHeaders();

		//! ImGui::TableNextColumn()
		void NextColumn();

	private:
		//! The flags submitted to the table. For custom flags in our implementation.
		FLAG_TABLE tableFlags;
	};

#pragma endregion // Table

#pragma region Alignment & Spacing

	/*****************************************************************//*!
	\class Indent
	\brief
		Wraps ImGui::Indent() and ImGui::Unindent().
	*//******************************************************************/
	class Indent
	{
	public:
		/*****************************************************************//*!
		\brief
			Constructor. Wraps ImGui::Indent() and ImGui::Unindent().
		\param amt
			The amount to indent by.
			If <= 0.0f, uses default indent width.
		*//******************************************************************/
		Indent(float amt = 0.0f);

		/*****************************************************************//*!
		\brief
			Destructor.
		*//******************************************************************/
		~Indent();

	private:
		float indentAmt;
	};

	//! ImGui::Separator()
	void Separator();
	//! ImGui::Spacing()
	void Spacing();
	//! ImGui::SameLine()
	void SameLine(float offset_from_start_x = 0.0f, float spacing = -1.0f);

	//! ImGui::GetContentRegionAvail()
	Vec2 GetAvailableContentRegion();
	//! ImGui::GetItemRectSize()
	Vec2 GetPrevItemRectSize();
	//! ImGui::GetItemRectMin()
	Vec2 GetPrevItemRectMin();
	//! ImGui::GetItemRectMax()
	Vec2 GetPrevItemRectMax();

	//! ImGui::SetNextItemWidth()
	void SetNextItemWidth(float width);

	//! ImGui::GetTextLineHeightWithSpacing()
	float GetTextLineHeightWithSpacing();

#pragma endregion // Alignment & Spacing

#pragma region Styling

	/*****************************************************************//*!
	\class SetStyleColor
	\brief
		Wraps ImGui::PushStyleColor() and ImGui::PopStyleColor().
	*//******************************************************************/
	class SetStyleColor : public internal::BeginEndBound_StyleColor
	{
	public:
		/*****************************************************************//*!
		\brief
			Constructor. Calls ImGui::PushStyleColor().
		\param apply
			Whether to actually apply the style color. For flexibility in code flow.
		*//******************************************************************/
		SetStyleColor(FLAG_STYLE_COLOR idx, const Vec4& val, bool apply = true);
	};

	/*****************************************************************//*!
	\class UnsetStyleColor
	\brief
		Equivalent to calling ImGui::PopStyleColor() for the specified number of times.
		When this goes out of scope, effectively calls ImGui::PushStyleColor() pushing
		the popped style colors back onto the stack.
	*//******************************************************************/
	class UnsetStyleColor
	{
	public:
		/*****************************************************************//*!
		\brief
			Constructor.
			Calls ImGui::PopStyleColor() the specified number of times, saving the
			style color to a buffer.
		\param numPops
			The number of style colors to pop from the stack.
		*//******************************************************************/
		UnsetStyleColor(unsigned int numPops);

		/*****************************************************************//*!
		\brief
			Destructor.
			Calls ImGui::PushStyleColor() pushing the style colors within the buffer
			back onto the stack.
		*//******************************************************************/
		~UnsetStyleColor();

	private:
#ifndef IMGUI_DISABLE
		//! The buffer storing popped style colors.
		std::vector<ImGuiColorMod> popped;
#endif

	};

	/*****************************************************************//*!
	\class SetStyleVar
	\brief
		Wraps ImGui::PushStyleVar() and ImGui::PopStyleVar().
	*//******************************************************************/
	class SetStyleVar : public internal::BeginEndBound_StyleVar
	{
	public:
		//! ImGui::PushStyleVar()
		SetStyleVar(FLAG_STYLE_VAR idx, const Vec2& val);
	};

	/*****************************************************************//*!
	\class UnsetStyleVar
	\brief
		Equivalent to calling ImGui::PopStyleVar() for the specified number of times.
		When this goes out of scope, effectively calls ImGui::PushStyleVar() pushing
		the popped style vars back onto the stack.
	*//******************************************************************/
	class UnsetStyleVar
	{
	public:
		/*****************************************************************//*!
		\brief
			Constructor.
			Calls ImGui::PopStyleVar() the specified number of times, saving the
			style var to a buffer.
		\param numPops
			The number of style vars to pop from the stack.
		*//******************************************************************/
		UnsetStyleVar(unsigned int numPops);

		/*****************************************************************//*!
		\brief
			Destructor.
			Calls ImGui::PushStyleVar() pushing the style vars within the buffer
			back onto the stack.
		*//******************************************************************/
		~UnsetStyleVar();

	private:
#ifndef IMGUI_DISABLE
		//! The buffer storing popped style vars.
		std::vector<ImGuiStyleMod> popped;
#endif

	};

	//! ImGui::GetStyleColorVec4()
	Vec4 GetStyleColor(FLAG_STYLE_COLOR style);
	//! ImGui::GetStyleColorVec4() with r,g,b and a multiplied
	Vec4 GetStyleColor(FLAG_STYLE_COLOR style, float channelMult, float alphaMult = 1.0f);
	//! ImGui::GetStyle().FramePadding
	Vec2 GetStyleFramePadding();

#pragma endregion // Styling

#pragma region Images

	// ImGui::Image()
	void Image(TextureID textureID, Vec2 size);

#pragma endregion // Images

#pragma region Graphs

	// ImGui::PlotLines()
	template <typename ContType>
		requires util::ConvertibleToCArray<ContType> && std::is_same_v<typename ContType::value_type, float>
	void PlotLines(internal::TextType label, const ContType& cont, Vec2 graphSize, float scaleMin = std::numeric_limits<float>::max(), float scaleMax = std::numeric_limits<float>::max(), const char* overlayText = nullptr);

#pragma endregion

#pragma region Custom Drawables

	//! ImGui::GetCursorScreenPos()
	Vec2 GetScreenCursorPos();
	//! ImGui::SetCursorPosX()
	void SetDrawCursorPosX(float x);
	//! ImGui::SetCursorPosY()
	void SetDrawCursorPosY(float y);
	//! ImGui::SetCursorPos()
	void SetDrawCursorPos(Vec2 pos);

	//! ImGui::GetWindowDrawList()->AddLine()
	void DrawLine(Vec2 p0, Vec2 p1, const Vec4& color);
	//! ImGui::GetWindowDrawList()->AddTriangleFilled()
	void DrawTriangle(Vec2 p0, Vec2 p1, Vec2 p2, const Vec4& color);
	//! ImGui::GetWindowDrawList()->AddRectFilled()
	void DrawRect(Vec2 p0, Vec2 p1, const Vec4& color, float cornerRoundingAmt = 0.0f);
	//! ImGui::GetWindowDrawList()->AddCircleFilled()
	void DrawCircle(Vec2 center, float radius, const Vec4& color);
	//! ImGui::GetWindowDrawList()->AddText()
	void DrawText(internal::TextType text, Vec2 pos, const Vec4& color);

#pragma endregion // Custom Drawables

#pragma region Misc

	/*****************************************************************//*!
	\class Disabled
	\brief
		Disables all elements drawn while this object is alive. Users won't
		be able to interact with the elements.
	*//******************************************************************/
	class Disabled : public internal::BeginEndBound_Disabled
	{
	public:
		//! ImGui::BeginDisabled()
		Disabled(bool isDisabled = true);
	};

	/*****************************************************************//*!
	\class Enabled
	\brief
		Disables the previous Disabled object, re-enabling elements drawn
		while this object is alive, until another Disabled object is created
		or this goes out of scope, where elements will be re-disabled.
	*//******************************************************************/
	class Enabled
	{
	public:
		//! ImGui::EndDisabled(), only calls if isEnabled is true and there was a previous ImGui::BeginDisabled(true) call
		Enabled(bool isEnabled = true);
		//! ImGui::BeginDisabled(), only calls if we did call ImGui::EndDisabled()
		~Enabled();

	private:
		bool didEnable;
	};

	/*****************************************************************//*!
	\class Renamable
	\brief
		Wraps a gui element with a context menu that allows for renaming
		the contents of that gui element.
	*//******************************************************************/
	class Renamable
	{
	public:
		// normalRoute is a function with no arguments.
		// onRenameRoute is a function with 1 const char* argument, which is the new name provided by the user.
		template <typename NormalFuncType, typename RenameCallbackFuncType>
			requires std::regular_invocable<NormalFuncType> && std::regular_invocable<RenameCallbackFuncType, const char*>
		static void Wrap(internal::TextType currName, NormalFuncType normalRoute, RenameCallbackFuncType onRenameRoute);

	private:
		Renamable() = delete;

	private:
		static char buffer[256];
		static int idOfItemBeingRenamed;
		static bool stealFocus;
	};

	//! ImGui::IsKeyPressed()
	bool IsKeyPressed(KEY key, bool repeating = true);
	//! ImGui::SetKeyboardFocusHere()
	void SetKeyboardFocusHere(int offset = 0);

	//! ImGui::IsMouseClicked()
	bool IsMouseClicked(MOUSE_BUTTON button);

	//! ImGui::SetScrollHereY()
	void SetScrollHereY(float center_y_ratio = 0.5f);

	//! ImGui::GetMainViewport()->Pos
	Vec2 GetMainViewportPos();
	//! ImGui::GetMainViewport()->Size
	Vec2 GetMainViewportSize();

#pragma endregion // Misc

}

#include "GUICollection.ipp"



// Include extensions here so users only have to include this main header to get both core functions and any extensions that they have enabled.
#ifdef GUI_EXTENSION_NOTIFICATIONS
#include "GUIExtension_Notifications.h"
#endif // GUI_EXTENSION_NOTIFICATIONS


#undef X
#undef X_DEPEND_DOCKING
