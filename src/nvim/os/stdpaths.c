#include <stdbool.h>

#include "nvim/os/stdpaths_defs.h"
#include "nvim/os/os.h"
#include "nvim/path.h"
#include "nvim/memory.h"

static const char *xdg_env_vars[] = {
  [kXDGConfigHome] = "XDG_CONFIG_HOME",
  [kXDGDataHome] = "XDG_DATA_HOME",
  [kXDGCacheHome] = "XDG_CACHE_HOME",
  [kXDGRuntimeDir] = "XDG_RUNTIME_DIR",
  [kXDGConfigDirs] = "XDG_CONFIG_DIRS",
  [kXDGDataDirs] = "XDG_DATA_DIRS",
};

static const char *const xdg_defaults[] = {
  // Windows, Apple stuff are just shims right now
#ifdef WIN32
  // Windows
#elif APPLE
  // Apple (this includes iOS, which we might need to handle differently)
  [kXDGConfigHome] = "~/Library/Preferences",
  [kXDGDataHome] = "~/Library/Application Support",
  [kXDGCacheHome] = "~/Library/Caches",
  [kXDGRuntimeDir] = "~/Library/Application Support",
  [kXDGConfigDirs] = "/Library/Application Support",
  [kXDGDataDirs] = "/Library/Application Support",
#else
  // Linux, BSD, CYGWIN
  [kXDGConfigHome] = "~/.config",
  [kXDGDataHome] = "~/.local/share",
  [kXDGCacheHome] = "~/.cache",
  [kXDGRuntimeDir] = "",
  [kXDGConfigDirs] = "/etc/xdg/",
  [kXDGDataDirs] = "/usr/local/share/:/usr/share/",
};
#endif

char *stdpaths_get_xdg_var(const XDGVarType idx)
  FUNC_ATTR_WARN_UNUSED_RESULT
{
  const char *const env = xdg_env_vars[idx];
  const char *const fallback = xdg_defaults[idx];

  const char *const env_val = os_getenv(env);
  char *ret = NULL;
  if (env_val != NULL) {
    ret = xstrdup(env_val);
  } else if (fallback) {
    ret = (char *) expand_env_save((char_u *)fallback);
  }

  return ret;
}

static char *get_xdg_home(const XDGVarType idx)
  FUNC_ATTR_WARN_UNUSED_RESULT
{
  char *dir = stdpaths_get_xdg_var(idx);
  if (dir) {
    dir = concat_fnames_realloc(dir, "nvim", true);
  }
  return dir;
}

char *stdpaths_user_conf_subpath(const char *fname)
  FUNC_ATTR_WARN_UNUSED_RESULT FUNC_ATTR_NONNULL_ALL
{
  return concat_fnames_realloc(get_xdg_home(kXDGConfigHome), fname, true);
}

char *stdpaths_user_data_subpath(const char *fname)
  FUNC_ATTR_WARN_UNUSED_RESULT FUNC_ATTR_NONNULL_ALL
{
  return concat_fnames_realloc(get_xdg_home(kXDGDataHome), fname, true);
}
