#include "GUIExtension_Notifications.h"
#ifdef GUI_EXTENSION_NOTIFICATIONS

namespace gui {
	namespace internal {

		class NotificationWithOpacity : public gui::Window
		{
		public:
			NotificationWithOpacity(gui::FLAG_WINDOW flags);

			uint64_t GetElapsedTime() const; // in milliseconds
			bool IsExpired() const;

		public:
			virtual void DrawContainer(int id) override;

		protected:
			std::chrono::system_clock::time_point createTime{ std::chrono::system_clock::now() };

		};

		class NotificationToast : public NotificationWithOpacity
		{
		public:
			NotificationToast(NotificationDetails&& details, uint64_t detailsHashHint = 0);

			bool IsSameContentAs(uint64_t otherDetailsHash) const;
			float GetWindowHeight() const;

			void CountUpDuplicate();

		public:
			void DrawContents() override;

		private:
			NotificationDetails details;
			uint64_t detailsHash; // Hash of the details to speed up duplicate comparison

			unsigned short duplicateCount{ 1 };

			float windowHeight{}; // Calculated after each window render
		};

		class NotificationHiddenMessage : public NotificationWithOpacity
		{
		public:
			NotificationHiddenMessage(int numHidden);

			void SetElapsedTime(uint64_t milliseconds);

		public:
			void DrawContents() override;

		private:
			int numHidden;
		};


		std::vector<NotificationToast> toasts;

	}
}

namespace gui {

	namespace internal {

		uint64_t NotificationDetails::CalcHash() const
		{
			return util::CombineHashes(
				util::Hash(content),
				util::CombineHashes(
					util::Hash(title),
					util::Hash(icon)
				)
			);
		}

		NotificationWithOpacity::NotificationWithOpacity(gui::FLAG_WINDOW flags)
			: Window{ "", gui::Vec2{}, flags }
		{
		}

		uint64_t NotificationWithOpacity::GetElapsedTime() const
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - createTime).count();
		}

		bool NotificationWithOpacity::IsExpired() const
		{
			return GetElapsedTime() >= GUI_NOTIFICATION_DURATION;
		}

		void NotificationWithOpacity::DrawContainer(int id)
		{
			auto Lerp{ [](const Vec4& a, const Vec4& b, float t) -> Vec4 {
			   return Vec4{
				   std::lerp(a.x, b.x, t),
				   std::lerp(a.y, b.y, t),
				   std::lerp(a.z, b.z, t),
				   std::lerp(a.w, b.w, t)
			   };
			} };

			uint64_t elapsedMs{ GetElapsedTime() };
			if (elapsedMs < GUI_NOTIFICATION_FADEIN_DURATION) // Fading in
			{
				gui::SetStyleColor windowBgColor{ gui::FLAG_STYLE_COLOR::WINDOW_BG, Lerp(gui::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f }, gui::GetStyleColor(gui::FLAG_STYLE_COLOR::WINDOW_BG), static_cast<float>(elapsedMs) / GUI_NOTIFICATION_FADEIN_DURATION) };

				Window::DrawContainer(id);
			}
			else // Hold and Fading out
			{
				float opacity{ std::min(static_cast<float>(GUI_NOTIFICATION_DURATION - elapsedMs) / GUI_NOTIFICATION_FADEOUT_DURATION, 1.0f) };
				gui::Vec4 color{ gui::GetStyleColor(gui::FLAG_STYLE_COLOR::TEXT, 1.0f, opacity) };
				gui::SetStyleColor textColor{ gui::FLAG_STYLE_COLOR::TEXT, color };
				color = gui::GetStyleColor(gui::FLAG_STYLE_COLOR::BUTTON, 1.0f, opacity);
				gui::SetStyleColor buttonColor{ gui::FLAG_STYLE_COLOR::BUTTON, color };
				gui::SetNextWindowBackgroundAlpha(opacity);

				Window::DrawContainer(id);
			}
		}

		NotificationToast::NotificationToast(NotificationDetails&& details, uint64_t detailsHashHint)
			: NotificationWithOpacity{ gui::FLAG_WINDOW::ALWAYS_AUTO_RESIZE | gui::FLAG_WINDOW::NO_DECORATION | gui::FLAG_WINDOW::NO_NAV | gui::FLAG_WINDOW::NO_BRING_TO_FRONT_ON_FOCUS | gui::FLAG_WINDOW::NO_FOCUS_ON_APPEARING }
			, details{ std::forward<NotificationDetails>(details) }
			, detailsHash{ detailsHashHint ? detailsHashHint : this->details.CalcHash() }
		{
		}

		bool NotificationToast::IsSameContentAs(uint64_t otherDetailsHash) const
		{
			return detailsHash == otherDetailsHash;
		}

		float NotificationToast::GetWindowHeight() const
		{
			return windowHeight;
		}

		void NotificationToast::CountUpDuplicate()
		{
			++duplicateCount;

			// Reset timer if flash is more than halfway done
			auto newTime{ std::chrono::system_clock::now() - std::chrono::milliseconds{ GUI_NOTIFICATION_FADEIN_DURATION / 2 } };
			if (createTime < newTime)
				createTime = newTime;
		}

		void NotificationToast::DrawContents()
		{
			gui::BringCurrWindowToFront();
			gui::SetTextWrapPos textWrapPos{ gui::GetMainViewportSize().x * 0.2f };

			// Render icon and title
			bool wasTitleRendered{};
			if (!details.icon.empty())
			{
				wasTitleRendered = true;
				gui::TextColored(details.iconColor, details.icon);
			}
			if (!details.title.empty())
			{
				if (wasTitleRendered)
					gui::SameLine();
				wasTitleRendered = true;
				gui::Text(details.title);
			}
			if (duplicateCount > 1)
			{
				if (wasTitleRendered)
					gui::SameLine();
				wasTitleRendered = true;
#ifdef GUI_FEATURE_CPP20_FORMAT
				gui::Text("({})", duplicateCount);
#else
				gui::Text("(%hu)", duplicateCount);
#endif
			}
			
			// Render dismiss button
			if (wasTitleRendered)
				gui:SameLine();
			gui::SetDrawCursorPosX(gui::GetWindowWidth() - GUI_NOTIFICATION_DISMISS_WIDTH - gui::GetStyleFramePadding().x * 2.0f);
#ifdef GUI_FEATURE_NOTIFICATION_IFA
			if (gui::Button(ICON_FA_XMARK, gui::Vec2{ GUI_NOTIFICATION_DISMISS_WIDTH, 0.0f }))
#else
			if (gui::Button("X", gui::Vec2{GUI_NOTIFICATION_DISMISS_WIDTH, 0.0f}))
#endif
				createTime -= std::chrono::milliseconds{ GUI_NOTIFICATION_DURATION };

			// Render message content
			gui::TextWrapped(details.content);

			// Record window height
			windowHeight = gui::GetWindowHeight();
		}

		NotificationHiddenMessage::NotificationHiddenMessage(int numHidden)
			: NotificationWithOpacity{ gui::FLAG_WINDOW::ALWAYS_AUTO_RESIZE | gui::FLAG_WINDOW::NO_DECORATION | gui::FLAG_WINDOW::NO_NAV | gui::FLAG_WINDOW::NO_BRING_TO_FRONT_ON_FOCUS | gui::FLAG_WINDOW::NO_FOCUS_ON_APPEARING | gui::FLAG_WINDOW::NO_INPUTS }
			, numHidden{ numHidden }
		{
		}

		void NotificationHiddenMessage::SetElapsedTime(uint64_t milliseconds)
		{
			createTime = std::chrono::system_clock::now() - std::chrono::milliseconds{ milliseconds };
		}

		void NotificationHiddenMessage::DrawContents()
		{
			gui::BringCurrWindowToFront();

#ifdef GUI_FEATURE_CPP20_FORMAT
			gui::Text("And {} more...", numHidden);
#else
			gui::Text("And %d more...", numHidden);
#endif
		}

		void SetNotificationIconAndTitle(NotificationDetails* content, FLAG_NOTIF type)
		{
			switch (type)
			{
			case FLAG_NOTIF::FATAL:
#ifdef GUI_FEATURE_NOTIFICATION_IFA
				content->icon = ICON_FA_BURST;
				content->iconColor = gui::Vec4{ 1.0f, 0.0f, 0.0f, 1.0f }; // Red
#endif
				content->title = "Fatal Error";
				break;
			case FLAG_NOTIF::ERROR:
#ifdef GUI_FEATURE_NOTIFICATION_IFA
				content->icon = ICON_FA_CIRCLE_EXCLAMATION;
				content->iconColor = gui::Vec4{ 1.0f, 0.0f, 0.0f, 1.0f }; // Red
#endif
				content->title = "Error";
				break;
			case FLAG_NOTIF::WARNING:
#ifdef GUI_FEATURE_NOTIFICATION_IFA
				content->icon = ICON_FA_TRIANGLE_EXCLAMATION;
				content->iconColor = gui::Vec4{ 1.0f, 1.0f, 0.0f, 1.0f }; // Yellow
#endif
				content->title = "Warning";
				break;
			case FLAG_NOTIF::INFO:
#ifdef GUI_FEATURE_NOTIFICATION_IFA
				content->icon = ICON_FA_CIRCLE_INFO;
				content->iconColor = gui::Vec4{ 0.0f, 0.6f, 1.0f, 1.0f }; // Blue
#endif
				content->title = "Info";
				break;
			case FLAG_NOTIF::DEBUG:
#ifdef GUI_FEATURE_NOTIFICATION_IFA
				content->icon = ICON_FA_COMMENT_DOTS;
				content->iconColor = gui::Vec4{ 0.5f, 0.5f, 0.5f, 1.0f }; // Gray
#endif
				content->title = "Debug";
				break;
			case FLAG_NOTIF::SUCCESS:
#ifdef GUI_FEATURE_NOTIFICATION_IFA
				content->icon = ICON_FA_CIRCLE_CHECK;
				content->iconColor = gui::Vec4{ 0.0f, 1.0f, 0.0f, 1.0f }; // Green
#endif
				content->title = "Success";
				break;
			case FLAG_NOTIF::FAILURE:
#ifdef GUI_FEATURE_NOTIFICATION_IFA
				content->icon = ICON_FA_CIRCLE_XMARK;
				content->iconColor = gui::Vec4{ 1.0f, 0.0f, 0.0f, 1.0f }; // Red
#endif
				content->title = "Failure";
				break;
#ifdef GUI_FEATURE_NOTIFICATION_IFA
			default:
				content->icon = ICON_FA_CIRCLE_QUESTION;
				content->iconColor = gui::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f }; // White
#endif
			}
		}

		void InsertNotification(NotificationDetails&& content)
		{
			uint64_t hash{ content.CalcHash() };
			for (NotificationToast& toast : toasts)
				if (toast.IsSameContentAs(hash))
				{
					toast.CountUpDuplicate();
					return;
				}

			toasts.emplace_back(std::forward<NotificationDetails>(content), hash);
		}

	}

	void RenderNotifications()
	{
#ifndef IMGUI_DISABLE

		gui::SetID toastsId{ "toasts" };

		const gui::Vec2 mainWindowPos{ gui::GetMainViewportPos() };
		const gui::Vec2 mainWindowSize{ gui::GetMainViewportSize() };

		int toastsProcessed{};
		uint64_t leastElapsedTimeOfToastsNotProcessed{ std::numeric_limits<int>::max() };
		float height{};

		// Erase expired toasts and Render visible toasts
		for (auto toastIter{ internal::toasts.begin() }; toastIter != internal::toasts.end(); )
		{
			if (toastIter->IsExpired())
			{
				toastIter = internal::toasts.erase(toastIter);
				continue;
			}
			++toastsProcessed;

			if constexpr (GUI_NOTIFICATION_MAX_SHOWN)
				if (toastsProcessed > GUI_NOTIFICATION_MAX_SHOWN)
				{
					leastElapsedTimeOfToastsNotProcessed = std::min(leastElapsedTimeOfToastsNotProcessed, toastIter->GetElapsedTime());
					++toastIter;
					continue;
				}

			gui::SetNextWindowPos(mainWindowPos + mainWindowSize - GUI_NOTIFICATION_PADDING - gui::Vec2{ 0.0f, height }, gui::FLAG_COND::NONE, gui::Vec2{ 1.0f, 1.0f });
			toastIter->Draw(toastsProcessed);

			height += toastIter->GetWindowHeight() + GUI_NOTIFICATION_SPACING_BETWEEN_NOTIFS;
			++toastIter;
		}

		// Draw an extra window showing hidden toasts if there are any
		if constexpr (GUI_NOTIFICATION_MAX_SHOWN)
			if (toastsProcessed > GUI_NOTIFICATION_MAX_SHOWN)
			{
				gui::SetNextWindowPos(mainWindowPos + mainWindowSize - GUI_NOTIFICATION_PADDING - gui::Vec2{ 0.0f, height }, gui::FLAG_COND::NONE, gui::Vec2{ 1.0f, 1.0f });
				internal::NotificationHiddenMessage additionalToastsMsg{ toastsProcessed - GUI_NOTIFICATION_MAX_SHOWN };
				additionalToastsMsg.SetElapsedTime(leastElapsedTimeOfToastsNotProcessed);
				additionalToastsMsg.Draw(GUI_NOTIFICATION_MAX_SHOWN + 1);
			}

#endif
	}

}

#endif // GUI_EXTENSION_NOTIFICATIONS
