#ifndef __GEGL_CONST_MULT_H__
#define __GEGL_CONST_MULT_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "gegl-point-op.h"

#define GEGL_TYPE_CONST_MULT               (gegl_const_mult_get_type ())
#define GEGL_CONST_MULT(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GEGL_TYPE_CONST_MULT, GeglConstMult))
#define GEGL_CONST_MULT_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass),  GEGL_TYPE_CONST_MULT, GeglConstMultClass))
#define GEGL_IS_CONST_MULT(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEGL_TYPE_CONST_MULT))
#define GEGL_IS_CONST_MULT_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass),  GEGL_TYPE_CONST_MULT))
#define GEGL_CONST_MULT_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj),  GEGL_TYPE_CONST_MULT, GeglConstMultClass))

#ifndef __TYPEDEF_GEGL_CONST_MULT__
#define __TYPEDEF_GEGL_CONST_MULT__
typedef struct _GeglConstMult GeglConstMult;
#endif
struct _GeglConstMult {
   GeglPointOp __parent__;

   /*< private >*/
   gfloat multiplier;
};

typedef struct _GeglConstMultClass GeglConstMultClass;
struct _GeglConstMultClass {
   GeglPointOpClass __parent__;
};

GType           gegl_const_mult_get_type       (void);
gfloat          gegl_const_mult_get_multiplier (GeglConstMult * self);
void            gegl_const_mult_set_multiplier (GeglConstMult * self, 
                                                gfloat multiplier);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
