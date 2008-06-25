#include <glib.h>
#include <libxml/tree.h>

xmlNodePtr dae_xml_get_child_by_tagname(xmlNodePtr parent, const gchar *tag)
{
	xmlNodePtr node;

	g_return_val_if_fail(parent != NULL, NULL);

	node = parent->children;
	while(node != NULL) {
		if((node->type == XML_ELEMENT_NODE) &&
			(xmlStrcmp(node->name, (const xmlChar *)tag) == 0)) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}

