#include "codecnv/codecnv.h"

#define	oemtext_sjistooem	codecnv_sjistoeuc
#define	oemtext_oemtosjis	codecnv_euctosjis

#ifdef __cplusplus
#include <string>
namespace std
{
	typedef string oemstring;
}
#endif  /* __cplusplus */
