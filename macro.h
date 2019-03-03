#ifndef _MACRO_H
#define _MACRO_H

#define ifor(i, m) for (int i = 0; i < (m); i++)
#define ijfor(i, j, m, n) for (int i = 0; i < (m); i++) for (int j = 0; j < (n); j++)
#define inside(i, j, m, n) ( (i) >= 0 && (i) < (m) && (j) >= 0 && (j) < (n) )

#endif // _MACRO_H
