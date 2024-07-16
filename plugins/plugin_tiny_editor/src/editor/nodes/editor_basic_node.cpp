#include "editor_basic_node.hpp"

namespace editor::nodes
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		struct QuadOffsets
		{
			ImVec2 TopLeft, BottomLeft, BottomRight, TopRight;
		};

		//------------------------------------------------------------------------------------------------------------------------
		QuadOffsets CalculateQuadOffsets(const float side_length)
		{
			const float half_side = 0.5f * side_length;

			QuadOffsets offset;

			offset.TopLeft = ImVec2(-half_side, half_side);
			offset.BottomLeft = ImVec2(-half_side, -half_side);
			offset.BottomRight = ImVec2(half_side, -half_side);
			offset.TopRight = ImVec2(half_side, half_side);

			return offset;
		}

		//------------------------------------------------------------------------------------------------------------------------
		struct TriangleOffsets
		{
			ImVec2 TopLeft, BottomLeft, Right;
		};

		//------------------------------------------------------------------------------------------------------------------------
		TriangleOffsets CalculateTriangleOffsets(const float side_length)
		{
			// Calculates the Vec2 offsets from an equilateral triangle's midpoint to
			// its vertices. Here is how the left_offset and right_offset are
			// calculated.
			//
			// For an equilateral triangle of side length s, the
			// triangle's height, h, is h = s * sqrt(3) / 2.
			//
			// The length from the base to the midpoint is (1 / 3) * h. The length from
			// the midpoint to the triangle vertex is (2 / 3) * h.
			const float sqrt_3 = sqrtf(3.0f);
			const float left_offset = -0.1666666666667f * sqrt_3 * side_length;
			const float right_offset = 0.333333333333f * sqrt_3 * side_length;
			const float vertical_offset = 0.5f * side_length;

			TriangleOffsets offset;
			offset.TopLeft = ImVec2(left_offset, vertical_offset);
			offset.BottomLeft = ImVec2(left_offset, -vertical_offset);
			offset.Right = ImVec2(right_offset, 0.f);

			return offset;
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline ImRect GetNodeTitleRect(const ImNodeData& node)
		{
			ImRect expanded_title_rect = node.TitleBarContentRect;
			expanded_title_rect.Expand(node.LayoutStyle.Padding);

			return ImRect(
				expanded_title_rect.Min,
				expanded_title_rect.Min + ImVec2(node.Rect.GetWidth(), 0.f) +
				ImVec2(0.f, expanded_title_rect.GetHeight()));
		}

		//------------------------------------------------------------------------------------------------------------------------
		void DrawPinShape(const ImVec2& pin_pos, const ImPinData& pin, const ImU32 pin_color)
		{
			static const int CIRCLE_NUM_SEGMENTS = 8;

			switch (pin.Shape)
			{
			case ImNodesPinShape_Circle:
			{
				GImNodes->CanvasDrawList->AddCircle(
					pin_pos,
					GImNodes->Style.PinCircleRadius,
					pin_color,
					CIRCLE_NUM_SEGMENTS,
					GImNodes->Style.PinLineThickness);
			}
			break;
			case ImNodesPinShape_CircleFilled:
			{
				GImNodes->CanvasDrawList->AddCircleFilled(
					pin_pos, GImNodes->Style.PinCircleRadius, pin_color, CIRCLE_NUM_SEGMENTS);
			}
			break;
			case ImNodesPinShape_Quad:
			{
				const QuadOffsets offset = CalculateQuadOffsets(GImNodes->Style.PinQuadSideLength);
				GImNodes->CanvasDrawList->AddQuad(
					pin_pos + offset.TopLeft,
					pin_pos + offset.BottomLeft,
					pin_pos + offset.BottomRight,
					pin_pos + offset.TopRight,
					pin_color,
					GImNodes->Style.PinLineThickness);
			}
			break;
			case ImNodesPinShape_QuadFilled:
			{
				const QuadOffsets offset = CalculateQuadOffsets(GImNodes->Style.PinQuadSideLength);
				GImNodes->CanvasDrawList->AddQuadFilled(
					pin_pos + offset.TopLeft,
					pin_pos + offset.BottomLeft,
					pin_pos + offset.BottomRight,
					pin_pos + offset.TopRight,
					pin_color);
			}
			break;
			case ImNodesPinShape_Triangle:
			{
				const TriangleOffsets offset =
					CalculateTriangleOffsets(GImNodes->Style.PinTriangleSideLength);
				GImNodes->CanvasDrawList->AddTriangle(
					pin_pos + offset.TopLeft,
					pin_pos + offset.BottomLeft,
					pin_pos + offset.Right,
					pin_color,
					// NOTE: for some weird reason, the line drawn by AddTriangle is
					// much thinner than the lines drawn by AddCircle or AddQuad.
					// Multiplying the line thickness by two seemed to solve the
					// problem at a few different thickness values.
					2.f * GImNodes->Style.PinLineThickness);
			}
			break;
			case ImNodesPinShape_TriangleFilled:
			{
				const TriangleOffsets offset =
					CalculateTriangleOffsets(GImNodes->Style.PinTriangleSideLength);
				GImNodes->CanvasDrawList->AddTriangleFilled(
					pin_pos + offset.TopLeft,
					pin_pos + offset.BottomLeft,
					pin_pos + offset.Right,
					pin_color);
			}
			break;
			default:
				IM_ASSERT(!"Invalid PinShape value!");
				break;
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		ImVec2 GetScreenSpacePinCoordinates(
			const ImRect& node_rect,
			const ImRect& attribute_rect,
			const ImNodesAttributeType type)
		{
			IM_ASSERT(type == ImNodesAttributeType_Input || type == ImNodesAttributeType_Output);
			const float x = type == ImNodesAttributeType_Input
				? (node_rect.Min.x - GImNodes->Style.PinOffset)
				: (node_rect.Max.x + GImNodes->Style.PinOffset);
			return ImVec2(x, 0.5f * (attribute_rect.Min.y + attribute_rect.Max.y));
		}

		//------------------------------------------------------------------------------------------------------------------------
		void DrawPin(ImNodesEditorContext& editor, const int pin_idx)
		{
			ImPinData& pin = editor.Pins.Pool[pin_idx];
			const ImRect& parent_node_rect = editor.Nodes.Pool[pin.ParentNodeIdx].Rect;

			pin.Pos = GetScreenSpacePinCoordinates(parent_node_rect, pin.AttributeRect, pin.Type);

			ImU32 pin_color = pin.ColorStyle.Background;

			if (GImNodes->HoveredPinIdx == pin_idx)
			{
				pin_color = pin.ColorStyle.Hovered;
			}

			DrawPinShape(pin.Pos, pin, pin_color);
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cbasic_node::cbasic_node(int id)
	{
		internal_data(id);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cbasic_node::~cbasic_node()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cbasic_node::do_draw(ImNodesEditorContext& ctx, const int idx)
	{
		const ImNodeData& node = ctx.Nodes.Pool[idx];
		ImGui::SetCursorPos(node.Origin + ctx.Panning);

		const bool node_hovered =
			GImNodes->HoveredNodeIdx == idx &&
			ctx.ClickInteraction.Type != ImNodesClickInteractionType_BoxSelection;

		ImU32 node_background = node.ColorStyle.Background;
		ImU32 titlebar_background = node.ColorStyle.Titlebar;

		if (ctx.SelectedNodeIndices.contains(idx))
		{
			node_background = node.ColorStyle.BackgroundSelected;
			titlebar_background = node.ColorStyle.TitlebarSelected;
		}
		else if (node_hovered)
		{
			node_background = node.ColorStyle.BackgroundHovered;
			titlebar_background = node.ColorStyle.TitlebarHovered;
		}

		{
			// node base
			GImNodes->CanvasDrawList->AddRectFilled(
				node.Rect.Min, node.Rect.Max, node_background, node.LayoutStyle.CornerRounding);

			// title bar:
			if (node.TitleBarContentRect.GetHeight() > 0.f)
			{
				ImRect title_bar_rect = GetNodeTitleRect(node);

#if IMGUI_VERSION_NUM < 18200
				GImNodes->CanvasDrawList->AddRectFilled(
					title_bar_rect.Min,
					title_bar_rect.Max,
					titlebar_background,
					node.LayoutStyle.CornerRounding,
					ImDrawCornerFlags_Top);
#else
				GImNodes->CanvasDrawList->AddRectFilled(
					title_bar_rect.Min,
					title_bar_rect.Max,
					titlebar_background,
					node.LayoutStyle.CornerRounding,
					ImDrawFlags_RoundCornersTop);

#endif
			}

			if ((GImNodes->Style.Flags & ImNodesStyleFlags_NodeOutline) != 0)
			{
#if IMGUI_VERSION_NUM < 18200
				GImNodes->CanvasDrawList->AddRect(
					node.Rect.Min,
					node.Rect.Max,
					node.ColorStyle.Outline,
					node.LayoutStyle.CornerRounding,
					ImDrawCornerFlags_All,
					node.LayoutStyle.BorderThickness);
#else
				GImNodes->CanvasDrawList->AddRect(
					node.Rect.Min,
					node.Rect.Max,
					node.ColorStyle.Outline,
					node.LayoutStyle.CornerRounding,
					ImDrawFlags_RoundCornersAll,
					node.LayoutStyle.BorderThickness);
#endif
			}
		}

		for (int i = 0; i < node.PinIndices.size(); ++i)
		{
			DrawPin(ctx, node.PinIndices[i]);
		}

		if (node_hovered)
		{
			GImNodes->HoveredNodeIdx = idx;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cbasic_node::do_update(ImNodesEditorContext& ctx, const int idx)
	{

	}

} //- editor::nodes