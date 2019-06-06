#ifndef QT_VERSION_INFO_H_INCLUDED
#define QT_VERSION_INFO_H_INCLUDED

typedef struct {
	char *version_str;
	int minor;
} qt_version_info_t;

qt_version_info_t *qt_version_info_create(const char*);
void qt_version_info_destroy(qt_version_info_t*);

const char *qt_version_info_get_version(qt_version_info_t*);
int qt_version_info_get_minor(qt_version_info_t*);

#endif /* QT_VERSION_INFO_H_INCLUDED */
