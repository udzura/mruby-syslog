/*
** syslog.c - Syslog module
**
*/

#include <string.h>
#include <syslog.h>

#include <mruby.h>
#include <mruby/string.h>
#include <mruby/variable.h>

#define MRB_SYSLOG_IDENT_LIMIT 1024

static struct mrb_syslog_option {
  char ident[MRB_SYSLOG_IDENT_LIMIT];
  int opened;
  int options;
  int facility;
} mrb_syslog_class_option;

static mrb_value
reset_vars(mrb_state *mrb, mrb_value self)
{
  memset(mrb_syslog_class_option.ident, '\0', MRB_SYSLOG_IDENT_LIMIT);
  mrb_syslog_class_option.opened = FALSE;
  mrb_syslog_class_option.options = 0;
  mrb_syslog_class_option.facility = 0;
  return self;
}

mrb_value
mrb_f_syslog_open(mrb_state *mrb, mrb_value self)
{
  mrb_value doller0;
  char *ident;
  mrb_int len, facility, options;

  if (mrb_syslog_class_option.opened) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "syslog already open");
  }

  doller0 = mrb_gv_get(mrb, mrb_intern_lit(mrb, "$0"));
  if (mrb_string_p(doller0)) {
    ident = mrb_string_value_cstr(mrb, &doller0);
  } else {
    ident = strdup("mruby");
  }
  options = LOG_PID | LOG_CONS;
  facility = LOG_USER;
  mrb_get_args(mrb, "|sii", &ident, &len, &options, &facility);
  if (len >= MRB_SYSLOG_IDENT_LIMIT) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "ident size too long");
  }

  ident[len] = '\0';
  memset(mrb_syslog_class_option.ident, '\0', MRB_SYSLOG_IDENT_LIMIT);
  strncpy(mrb_syslog_class_option.ident, ident, len + 1);
  openlog(mrb_syslog_class_option.ident, options, facility);

  mrb_syslog_class_option.opened = TRUE;
  mrb_syslog_class_option.options = options;
  mrb_syslog_class_option.facility = facility;

  return self;
}

mrb_value
mrb_f_syslog_log0(mrb_state *mrb, mrb_value self)
{
  mrb_int len, prio;
  char *cp;

  mrb_get_args(mrb, "is", &prio, &cp, &len);
  syslog(prio, "%*s", len, cp);
  return self;
}

mrb_value
mrb_f_syslog_close(mrb_state *mrb, mrb_value self)
{
  if (!mrb_syslog_class_option.opened) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "syslog not opened");
  }

  closelog();
  reset_vars(mrb, self);

  return mrb_nil_value();
}

mrb_value
mrb_f_syslog_ident(mrb_state *mrb, mrb_value self)
{
  if (mrb_syslog_class_option.opened) {
    return mrb_str_new_cstr(mrb, mrb_syslog_class_option.ident);
  } else {
    return mrb_nil_value();
  }
}

void
mrb_mruby_syslog_gem_init(mrb_state *mrb)
{
  struct RClass *slog;

  slog = mrb_define_module(mrb, "Syslog");

  mrb_define_module_function(mrb, slog, "open", mrb_f_syslog_open, MRB_ARGS_ANY());
  mrb_define_module_function(mrb, slog, "_log0", mrb_f_syslog_log0, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, slog, "close", mrb_f_syslog_close, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, slog, "ident", mrb_f_syslog_ident, MRB_ARGS_NONE());

/* Syslog options */
#define mrb_define_syslog_option(c) mrb_define_const(mrb, slog, #c, mrb_fixnum_value(c))

#ifdef LOG_PID
  mrb_define_syslog_option(LOG_PID);
#endif
#ifdef LOG_CONS
  mrb_define_syslog_option(LOG_CONS);
#endif
#ifdef LOG_ODELAY
  mrb_define_syslog_option(LOG_ODELAY); /* deprecated */
#endif
#ifdef LOG_NDELAY
  mrb_define_syslog_option(LOG_NDELAY);
#endif
#ifdef LOG_NOWAIT
  mrb_define_syslog_option(LOG_NOWAIT); /* deprecated */
#endif
#ifdef LOG_PERROR
  mrb_define_syslog_option(LOG_PERROR);
#endif

/* Syslog facilities */
#define mrb_define_syslog_facility(c) mrb_define_const(mrb, slog, #c, mrb_fixnum_value(c))

#ifdef LOG_AUTH
  mrb_define_syslog_facility(LOG_AUTH);
#endif
#ifdef LOG_AUTHPRIV
  mrb_define_syslog_facility(LOG_AUTHPRIV);
#endif
#ifdef LOG_CONSOLE
  mrb_define_syslog_facility(LOG_CONSOLE);
#endif
#ifdef LOG_CRON
  mrb_define_syslog_facility(LOG_CRON);
#endif
#ifdef LOG_DAEMON
  mrb_define_syslog_facility(LOG_DAEMON);
#endif
#ifdef LOG_FTP
  mrb_define_syslog_facility(LOG_FTP);
#endif
#ifdef LOG_KERN
  mrb_define_syslog_facility(LOG_KERN);
#endif
#ifdef LOG_LPR
  mrb_define_syslog_facility(LOG_LPR);
#endif
#ifdef LOG_MAIL
  mrb_define_syslog_facility(LOG_MAIL);
#endif
#ifdef LOG_NEWS
  mrb_define_syslog_facility(LOG_NEWS);
#endif
#ifdef LOG_NTP
  mrb_define_syslog_facility(LOG_NTP);
#endif
#ifdef LOG_SECURITY
  mrb_define_syslog_facility(LOG_SECURITY);
#endif
#ifdef LOG_SYSLOG
  mrb_define_syslog_facility(LOG_SYSLOG);
#endif
#ifdef LOG_USER
  mrb_define_syslog_facility(LOG_USER);
#endif
#ifdef LOG_UUCP
  mrb_define_syslog_facility(LOG_UUCP);
#endif
#ifdef LOG_LOCAL0
  mrb_define_syslog_facility(LOG_LOCAL0);
#endif
#ifdef LOG_LOCAL1
  mrb_define_syslog_facility(LOG_LOCAL1);
#endif
#ifdef LOG_LOCAL2
  mrb_define_syslog_facility(LOG_LOCAL2);
#endif
#ifdef LOG_LOCAL3
  mrb_define_syslog_facility(LOG_LOCAL3);
#endif
#ifdef LOG_LOCAL4
  mrb_define_syslog_facility(LOG_LOCAL4);
#endif
#ifdef LOG_LOCAL5
  mrb_define_syslog_facility(LOG_LOCAL5);
#endif
#ifdef LOG_LOCAL6
  mrb_define_syslog_facility(LOG_LOCAL6);
#endif
#ifdef LOG_LOCAL7
  mrb_define_syslog_facility(LOG_LOCAL7);
#endif

/* Syslog levels and the shortcut methods */
#define mrb_define_syslog_level(c, m) mrb_define_const(mrb, slog, #c, mrb_fixnum_value(c));

#ifdef LOG_EMERG
  mrb_define_syslog_level(LOG_EMERG, emerg);
#endif
#ifdef LOG_ALERT
  mrb_define_syslog_level(LOG_ALERT, alert);
#endif
#ifdef LOG_CRIT
  mrb_define_syslog_level(LOG_CRIT, crit);
#endif
#ifdef LOG_ERR
  mrb_define_syslog_level(LOG_ERR, err);
#endif
#ifdef LOG_WARNING
  mrb_define_syslog_level(LOG_WARNING, warning);
#endif
#ifdef LOG_NOTICE
  mrb_define_syslog_level(LOG_NOTICE, notice);
#endif
#ifdef LOG_INFO
  mrb_define_syslog_level(LOG_INFO, info);
#endif
#ifdef LOG_DEBUG
  mrb_define_syslog_level(LOG_DEBUG, debug);
#endif
}

void
mrb_mruby_syslog_gem_final(mrb_state *mrb)
{
}
