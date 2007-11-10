/* This file is part of GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should prepare received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Copyright 2006 Øyvind Kolås
 */

#include "config.h"
#include <string.h>
#include <glib-object.h>

#include "gegl-types.h"
#include "gegl-prepare-visitor.h"
#include "graph/gegl-node.h"
#include "graph/gegl-pad.h"
#include "graph/gegl-visitable.h"
#include "gegl-instrument.h"
#include "operation/gegl-operation.h"


static void gegl_prepare_visitor_class_init (GeglPrepareVisitorClass *klass);
static void visit_node (GeglVisitor *self,
                        GeglNode    *node);


G_DEFINE_TYPE (GeglPrepareVisitor, gegl_prepare_visitor, GEGL_TYPE_VISITOR)


static void
gegl_prepare_visitor_class_init (GeglPrepareVisitorClass *klass)
{
  GeglVisitorClass *visitor_class = GEGL_VISITOR_CLASS (klass);

  visitor_class->visit_node = visit_node;
}

static void
gegl_prepare_visitor_init (GeglPrepareVisitor *self)
{
}

static void
visit_node (GeglVisitor *self,
            GeglNode    *node)
{
  GeglOperation *operation = node->operation;

  glong          time = gegl_ticks ();

  GEGL_VISITOR_CLASS (gegl_prepare_visitor_parent_class)->visit_node (self, node);

  if (self->context_id == NULL)
    g_warning ("hmm");
  gegl_node_add_dynamic (node, self->context_id);

  /* prepare the operation for the coming evaluation (all properties
   * should be set now).
   */
  {
    const gchar *name = gegl_node_get_name (node);
    if (name && !strcmp (name, "proxynop-output"))
      {
        GeglGraph *graph = g_object_get_data (G_OBJECT (node), "graph");
        g_assert (graph);
        if (GEGL_NODE (graph)->operation)
          {
            /* issuing a prepare on the graph, FIXME: we might need to do
             * a cycle of prepares as deep as the nesting of graphs,.
             * (or find a better way to do this) */
            gegl_operation_prepare (GEGL_NODE (graph)->operation, self->context_id);
          }
      }
  }

  gegl_operation_prepare (operation, self->context_id);
  gegl_node_set_need_rect (node, self->context_id, 0, 0, 0, 0);
  time = gegl_ticks () - time;
  gegl_instrument ("process", gegl_node_get_operation (node), time);
  gegl_instrument (gegl_node_get_operation (node), "prepare", time);
}
