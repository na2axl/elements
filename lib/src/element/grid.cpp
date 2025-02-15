/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/grid.hpp>
#include <elements/support/context.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Vertical Grids
   ////////////////////////////////////////////////////////////////////////////
   view_limits vgrid_element::limits(basic_context const& ctx) const
   {
      _num_spans = 0;
      for (std::size_t i = 0; i != size(); ++i)
         _num_spans += at(i).span();

      view_limits limits{{0.0, 0.0}, {full_extent, 0.0}};
      std::size_t gi = 0;
      float prev = 0;
      float desired_total_min = 0;

      for (std::size_t i = 0; i != size();  ++i)
      {
         auto& elem = at(i);
         gi += elem.span()-1;
         auto y = grid_coord(gi++);
         auto height = y - prev;
         auto factor = 1.0/height;
         prev = y;

         auto el = elem.limits(ctx);
         auto elem_desired_total_min = el.min.y * factor;
         if (desired_total_min < elem_desired_total_min)
            desired_total_min = elem_desired_total_min;

         limits.max.y += el.max.y;
         clamp_min(limits.min.x, el.min.x);
         clamp_max(limits.max.x, el.max.x);
      }

      limits.min.y = desired_total_min;
      clamp_min(limits.max.x, limits.min.x);
      clamp_max(limits.max.y, full_extent);
      return limits;
   }

   void vgrid_element::layout(context const& ctx)
   {
      _positions.resize(size()+1);

      auto left = ctx.bounds.left;
      auto right = ctx.bounds.right;
      auto top = ctx.bounds.top;
      auto total_height = ctx.bounds.height();
      std::size_t gi = 0;

      float prev = 0;
      for (std::size_t i = 0; i != size(); ++i)
      {
         auto& elem = at(i);
         gi += elem.span()-1;
         auto y = grid_coord(gi++) * total_height;
         auto height = y - prev;
         rect ebounds = {left, prev+top, right, prev+top+height};
         elem.layout(context{ctx, &elem, ebounds});
         _positions[i] = prev+top;
         prev = y;
      }
      _positions[size()] = total_height+top;
   }

   rect vgrid_element::bounds_of(context const& ctx, std::size_t index) const
   {
      if (index >= size())
         return {};
      auto left = ctx.bounds.left;
      auto right = ctx.bounds.right;
      return {left, _positions[index], right, _positions[index+1]};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Horizontal Grids
   ////////////////////////////////////////////////////////////////////////////
   view_limits hgrid_element::limits(basic_context const& ctx) const
   {
      _num_spans = 0;
      for (std::size_t i = 0; i != size(); ++i)
         _num_spans += at(i).span();

      view_limits limits{{ 0.0, 0.0}, {0.0, full_extent}};
      std::size_t gi = 0;
      float prev = 0;
      float desired_total_min = 0;

      for (std::size_t i = 0; i != size();  ++i)
      {
         auto& elem = at(i);
         gi += elem.span()-1;
         auto x = grid_coord(gi++);
         auto width = x - prev;
         auto factor = 1.0/width;
         prev = x;

         auto el = elem.limits(ctx);
         auto elem_desired_total_min = el.min.x * factor;
         if (desired_total_min < elem_desired_total_min)
            desired_total_min = elem_desired_total_min;

         limits.max.x += el.max.x;
         clamp_min(limits.min.y, el.min.y);
         clamp_max(limits.max.y, el.max.y);
      }

      limits.min.x = desired_total_min;
      clamp_min(limits.max.y, limits.min.y);
      clamp_max(limits.max.x, full_extent);
      return limits;
   }

   void hgrid_element::layout(context const& ctx)
   {
      _positions.resize(size()+1);

      auto top = ctx.bounds.top;
      auto bottom = ctx.bounds.bottom;
      auto left = ctx.bounds.left;
      auto total_width = ctx.bounds.width();
      std::size_t gi = 0;

      float prev = 0;
      for (std::size_t i = 0; i != size(); ++i)
      {
         auto& elem = at(i);
         gi += elem.span()-1;
         auto x = grid_coord(gi++) * total_width;
         auto width = x - prev;
         rect ebounds = {prev+left, top, prev+left+width, bottom};
         elem.layout(context{ctx, &elem, ebounds});
         _positions[i] = prev+left;
         prev = x;
      }
      _positions[size()] = total_width+left;
   }

   rect hgrid_element::bounds_of(context const& ctx, std::size_t index) const
   {
      if (index >= size())
         return {};
      auto top = ctx.bounds.top;
      auto bottom = ctx.bounds.bottom;
      return {_positions[index], top, _positions[index+1], bottom};
   }
}}
