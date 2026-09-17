#pragma once
// Yield a room operation between its HTTP steps. Completed responses stay cached
// until that operation finishes, so replaying it never repeats a POST.
struct BrowserHttpPending {};
void clear_browser_http();
