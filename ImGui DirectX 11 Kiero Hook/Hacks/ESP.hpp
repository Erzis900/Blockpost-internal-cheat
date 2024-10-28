#pragma once
#include "../includes.h"

namespace ESP
{
	bool WorldToScreen(Unity::Vector3& worldPos, Unity::Vector2& screenPos)
	{
		if (!UnityVars::cam)
			return false;

		Unity::Vector3 temp;
		UnityVars::cam->WorldToScreen(worldPos, temp);

		if (temp.x > vars::screenSize.x || temp.x < 0 || temp.y > vars::screenSize.y || temp.y < 0 || temp.z < 0)
		{
			return false;
		}

		if (temp.z > 0)
		{
			screenPos = Unity::Vector2(temp.x, vars::screenSize.y - temp.y);
		}

		if (screenPos.x > 0 || screenPos.y > 0)
		{
			return true;
		}

		return false;
	}

    void Draw(Unity::Vector2& screenPos, Unity::Vector3& entityPos, std::string& entityName) {
        if (vars::isSnaplineEnabled) {
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vars::screenCenter.x, vars::screenSize.y),
                ImVec2(screenPos.x, screenPos.y),
                IM_COL32(
                    static_cast<int>(vars::snaplineColor.x * 255),
                    static_cast<int>(vars::snaplineColor.y * 255),
                    static_cast<int>(vars::snaplineColor.z * 255),
                    static_cast<int>(vars::snaplineColor.w * 255)
                ));
        }

        if (vars::isEspEnabled || vars::isNames) {
            Unity::Vector3 entityBottom = { entityPos.x, entityPos.y, entityPos.z };
            Unity::Vector3 entityTop = { entityPos.x, entityPos.y + vars::entityHeight, entityPos.z };

            Unity::Vector2 screenBottom, screenTop;
            if (ESP::WorldToScreen(entityBottom, screenBottom) && ESP::WorldToScreen(entityTop, screenTop)) {
                float boxHeight = screenBottom.y - screenTop.y;
                float boxWidth = boxHeight / 2.0f;

                if (vars::isEspEnabled)
                {
                    ImGui::GetBackgroundDrawList()->AddRect(
                        ImVec2(screenTop.x - boxWidth / 2, screenTop.y),
                        ImVec2(screenTop.x + boxWidth / 2, screenBottom.y),
                        IM_COL32(
                            static_cast<int>(vars::espColor.x * 255),
                            static_cast<int>(vars::espColor.y * 255),
                            static_cast<int>(vars::espColor.z * 255),
                            static_cast<int>(vars::espColor.w * 255)
                        )
                    );
                }
                
                if (vars::isNames)
                {
                    ImGui::GetBackgroundDrawList()->AddText(
                        ImVec2(screenTop.x - boxWidth / 2, screenTop.y - 15),
                        IM_COL32(255, 255, 255, 255),
                        entityName.c_str()
                    );
                }
            }
        }
    }
};