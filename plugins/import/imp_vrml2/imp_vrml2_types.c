#include <string.h>

#include "imp_vrml_types.h"
#include "imp_vrml_read.h"

VrmlType vrml2_type_cb__float3(VrmlReader *reader)
{
	if(!vrml_read_skip_ws(reader))
		return T_UNKNOWN;
	if(*(reader->bufp) == '[')
		return T_LIST_FLOAT3;
	return T_FLOAT3;
}

VrmlType vrml2_type_cb__int(VrmlReader *reader)
{
	if(!vrml_read_skip_ws(reader))
		return T_UNKNOWN;
	if(*(reader->bufp) == '[')
		return T_LIST_INT;
	return T_INT;
}

VrmlType vrml2_type_cb_field(VrmlReader *reader)
{
	gchar *id;
	VrmlType t = T_UNKNOWN;

	id = vrml_read_id(reader);
	if(!id) {
		g_warning("VRML: vrml2_type_cb_field: failed to read id");
		return T_UNKNOWN;
	}
	if(strcmp(id, "SFFloat") == 0)
		t = T_TOKEN2; /* data 0 */

	g_free(id);
	return t;
}

VrmlType vrml2_type_cb_children(VrmlReader *reader)
{
	if(!vrml_read_skip_ws(reader))
		return T_UNKNOWN;
	if(*(reader->bufp) == '[')
		return T_LIST_OBJECT;
	return T_OBJECT;
}

VrmlType vrml2_type_cb_color(VrmlReader *reader)
{
	if(!vrml_read_skip_ws(reader))
		return T_UNKNOWN;
	if(*(reader->bufp) == '[')
		return T_LIST_FLOAT3;
	return T_OBJECT;
}
