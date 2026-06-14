#pragma once
#include "GUICollection.h"
#ifdef GUI_EXTENSION_NOTIFICATIONS

// This implementation is based off the following repos:
// https://github.com/TyomaVader/ImGuiNotify by TyomaVader
// https://github.com/patrickcjk/imgui-notify by patrickcjk
// Thank you so much for providing the inspiration for this feature!



// Enable/Disable features
#define GUI_FEATURE_NOTIFICATION_IFA						// Use IconsFontAwesome

// Customize features
#define GUI_NOTIFICATION_MAX_SHOWN 5						// Maximum toasts rendered at once. Set to 0 for unlimited.
#define GUI_NOTIFICATION_DURATION 5000						// Time until expiry in milliseconds
#define GUI_NOTIFICATION_FADEIN_DURATION 500				// Time until fully faded-in in milliseconds
#define GUI_NOTIFICATION_FADEOUT_DURATION 350				// Time until fully faded-out in milliseconds
#define GUI_NOTIFICATION_PADDING gui::Vec2{ 20.0f, 20.0f }	// Padding from the edge of the screen
#define GUI_NOTIFICATION_SPACING_BETWEEN_NOTIFS 10.0f		// Vertical spacing between notifications
#define GUI_NOTIFICATION_DISMISS_WIDTH 20.0f				// Width of the dismiss button



#ifdef GUI_FEATURE_NOTIFICATION_IFA
#include "IconsFontAwesome6.h"
#endif


namespace gui {

	/*****************************************************************//*!
	\enum class FLAG_NOTIF
	\brief
		Predefined types of notifications
	*//******************************************************************/
	enum class FLAG_NOTIF : int {
		FATAL,
		ERROR,
		WARNING,
		INFO,
		DEBUG,
		SUCCESS,
		FAILURE
	};
	// No bitwise operators for this flag as they aren't supposed to be combined (these aren't binary flags)

	// Dispatch a notification
	template <typename ...Args>
	void Notification(FLAG_NOTIF type, internal::TextType fmt, const Args&... args);
	template <typename ...Args>
	void Notification(internal::TextType title, internal::TextType contentFmt, const Args&... args); // TODO: Support custom icons

	// Render the notifications
	void RenderNotifications();

}



#pragma region Implementation

namespace gui {
	namespace internal {

		struct NotificationDetails
		{
			std::string icon;
			gui::Vec4 iconColor;
			std::string title;
			std::string content;

			uint64_t CalcHash() const;
		};

		void SetNotificationIconAndTitle(NotificationDetails* content, FLAG_NOTIF type);
		void InsertNotification(NotificationDetails&& content);

	}
}

namespace gui {

	template<typename ...Args>
	void Notification([[maybe_unused]] FLAG_NOTIF type, [[maybe_unused]] internal::TextType fmt, [[maybe_unused]] const Args&... args)
	{
#ifndef IMGUI_DISABLE
		internal::NotificationDetails content;
		internal::SetNotificationIconAndTitle(&content, type);
		content.content = util::sprintf(fmt, args...);

		internal::InsertNotification(std::move(content));
#endif
	}

	template<typename ...Args>
	void Notification([[maybe_unused]] internal::TextType title, [[maybe_unused]] internal::TextType contentFmt, [[maybe_unused]] const Args& ...args)
	{
#ifndef IMGUI_DISABLE
		internal::NotificationDetails content;
		content.title = title;
		content.content = util::sprintf(contentFmt, args...);

		internal::InsertNotification(std::move(content));
#endif
	}

}

#pragma endregion // Implementation

#endif // GUI_EXTENSION_NOTIFICATIONS
