/* ====================================================================
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 2000 The Apache Software Foundation.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "Apache" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 */

#ifndef APR_THREAD_PROC_H
#define APR_THREAD_PROC_H

#include "apr_file_io.h"
#include "apr_general.h"
#include "apr_errno.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {APR_SHELLCMD, APR_PROGRAM} ap_cmdtype_e;
typedef enum {APR_WAIT, APR_NOWAIT} ap_wait_how_e;

#define APR_NO_PIPE          0
#define APR_FULL_BLOCK       1
#define APR_FULL_NONBLOCK    2
#define APR_PARENT_BLOCK     3
#define APR_CHILD_BLOCK      4

#define APR_CANCEL_ASYNCH      1
#define APR_CANCEL_DEFER       2
#define APR_CANCEL_ENABLE      3
#define APR_CANCEL_DISABLE     4

#define APR_OC_REASON_DEATH         0     /* child has died, caller must call
                                           * unregister still */
#define APR_OC_REASON_UNWRITABLE    1     /* write_fd is unwritable */
#define APR_OC_REASON_RESTART       2     /* a restart is occuring, perform
                                           * any necessary cleanup (including
                                           * sending a special signal to child)
                                           */
#define APR_OC_REASON_UNREGISTER    3     /* unregister has been called, do
                                           * whatever is necessary (including
                                           * kill the child) */
#define APR_OC_REASON_LOST          4     /* somehow the child exited without
                                           * us knowing ... buggy os? */

typedef struct ap_thread_t           ap_thread_t;
typedef struct ap_threadattr_t       ap_threadattr_t;
typedef struct ap_proc_t		  ap_proc_t;
typedef struct ap_procattr_t         ap_procattr_t;

typedef struct ap_threadkey_t        ap_threadkey_t;
typedef struct ap_other_child_rec_t  ap_other_child_rec_t;

typedef void *(API_THREAD_FUNC *ap_thread_start_t)(void *);

/* Thread Function definitions */
/* ***APRDOC********************************************************
 * ap_status_t ap_create_threadattr(ap_threadattr_t **new, ap_context_t *cont)
 *    Create and initialize a new threadattr variable
 * arg 1) The newly created threadattr.
 * arg 2) The context to use
 */
ap_status_t ap_create_threadattr(ap_threadattr_t **new, ap_context_t *cont);

/* ***APRDOC********************************************************
 * ap_status_t ap_setthreadattr_detach(ap_threadattr_t *attr, ap_int32_t on)
 *    Set if newly created threads should be created in detach mode.
 * arg 1) The threadattr to affect 
 * arg 2) Thread detach state on or off
 */
ap_status_t ap_setthreadattr_detach(ap_threadattr_t *attr, ap_int32_t on);

/* ***APRDOC********************************************************
 * ap_status_t ap_getthreadattr_detach(ap_threadattr_t *attr)
 *    Get the detach mode for this threadattr.
 * arg 1) The threadattr to reference 
 */
ap_status_t ap_getthreadattr_detach(ap_threadattr_t *iattr);

/* ***APRDOC********************************************************
 * ap_status_t ap_create_thread(ap_thread_t **new, ap_threadattr_t *attr, 
 *                              ap_thread_start_t func, void *data, 
 *                              ap_context_t *cont)
 *    Create a new thread of execution 
 * arg 1) The newly created thread handle.
 * arg 2) The threadattr to use to determine how to create the thread
 * arg 3) The function to start the new thread in
 * arg 4) Any data to be passed to the starting function
 * arg 5) The context to use
 */
ap_status_t ap_create_thread(ap_thread_t **new, ap_threadattr_t *attr, 
                             ap_thread_start_t func, void *data, 
                             ap_context_t *cont);

/* ***APRDOC********************************************************
 * ap_status_t ap_thread_exit(ap_thread_t *thd, ap_status_t *retval)
 *    stop the current thread 
 * arg 1) The thread to stop
 * arg 2) The return value to pass back to any thread that cares
 */
ap_status_t ap_thread_exit(ap_thread_t *thd, ap_status_t *retval);

/* ***APRDOC********************************************************
 * ap_status_t ap_thread_join(ap_status_t *retval, ap_thread_t *thd)
 *    block until the desired thread stops executing. 
 * arg 1) The return value from the dead thread.
 * arg 2) The thread to join
 */
ap_status_t ap_thread_join(ap_status_t *retval, ap_thread_t *thd); 

/* ***APRDOC********************************************************
 * ap_status_t ap_thread_detach(ap_thread_t *thd)
 *    detach a thread
 * arg 1) The thread to detach 
 */
ap_status_t ap_thread_detach(ap_thread_t *thd);

ap_status_t ap_cancel_thread(ap_thread_t *thd);
ap_status_t ap_setcanceltype(ap_int32_t type, ap_context_t *cont);
ap_status_t ap_setcancelstate(ap_int32_t type, ap_context_t *cont);

/* ***APRDOC********************************************************
 * ap_status_t ap_get_threaddata(void **data, char *key, ap_thread_t *thread)
 *    Return the context associated with the current thread.
 * arg 1) The user data associated with the thread.
 * arg 2) The key to associate with the data
 * arg 3) The currently open thread.
 */
ap_status_t ap_get_threaddata(void **data, char *key, ap_thread_t *thread);

/* ***APRDOC********************************************************
 * ap_status_t ap_set_threaddata(void *data, char *key,
                                 ap_status_t (*cleanup) (void *),
                                 ap_thread_t *thread)
 *    Return the context associated with the current thread.
 * arg 1) The user data to associate with the thread.
 * arg 2) The key to use for associating the data with the tread
 * arg 3) The cleanup routine to use when the thread is destroyed.
 * arg 4) The currently open thread.
 */
ap_status_t ap_set_threaddata(void *data, char *key,
                              ap_status_t (*cleanup) (void *), 
                              ap_thread_t *thread);

/* ***APRDOC********************************************************
 * ap_status_t ap_create_thread_private(ap_threadkey_t **key, 
 *                                      void (*dest)(void *),
 *                                      ap_context_t *cont)
 *    Create and initialize a new thread private address space
 * arg 1) The thread private handle.
 * arg 2) The destructor to use when freeing the private memory.
 * arg 3) The context to use
 */
ap_status_t ap_create_thread_private(ap_threadkey_t **key, void (*dest)(void *), 
                                     ap_context_t *cont);

/* ***APRDOC********************************************************
 * ap_status_t ap_get_thread_private(void **new, ap_threadkey_t *key)
 *    Get a pointer to the thread private memory
 * arg 1) The data stored in private memory 
 * arg 2) The handle for the desired thread private memory 
 */
ap_status_t ap_get_thread_private(void **new, ap_threadkey_t *key);

/* ***APRDOC********************************************************
 * ap_status_t ap_set_thread_private(void *priv, ap_threadkey_t *key)
 *    Set the data to be stored in thread private memory
 * arg 1) The data to be stored in private memory 
 * arg 2) The handle for the desired thread private memory 
 */
ap_status_t ap_set_thread_private(void *priv, ap_threadkey_t *key);

/* ***APRDOC********************************************************
 * ap_status_t ap_delete_thread_private(ap_threadkey_t *key)
 *    Free the thread private memory
 * arg 1) The handle for the desired thread private memory 
 */
ap_status_t ap_delete_thread_private(ap_threadkey_t *key);

/* ***APRDOC********************************************************
 * ap_status_t ap_get_threadkeydata(void **data, char *key, 
 *                                  ap_threadkey_t *threadkey)
 *    Return the context associated with the current threadkey.
 * arg 1) The user data associated with the threadkey.
 * arg 2) The key associated with the data
 * arg 3) The currently open threadkey.
 */
ap_status_t ap_get_threadkeydata(void **data, char *key, ap_threadkey_t *threadkey);

/* ***APRDOC********************************************************
 * ap_status_t ap_set_threadkeydata(void *data, char *key,
                                    ap_status_t (*cleanup) (void *),
                                    ap_threadkey_t *threadkey)
 *    Return the context associated with the current threadkey.
 * arg 1) The data to set.
 * arg 1) The key to associate with the data.
 * arg 2) The user data to associate with the threadkey.
 * arg 1) The currently open threadkey.
 */
ap_status_t ap_set_threadkeydata(void *data, char *key,
                                 ap_status_t (*cleanup) (void *), 
                                 ap_threadkey_t *threadkey);

/* Process Function definitions */
/* ***APRDOC********************************************************
 * ap_status_t ap_createprocattr_init(ap_procattr_t **new, ap_context_t *cont)
 *    Create and initialize a new procattr variable 
 * arg 1) The newly created procattr. 
 * arg 2) The context to use
 */
ap_status_t ap_createprocattr_init(ap_procattr_t **new, ap_context_t *cont);

/* ***APRDOC********************************************************
 * ap_status_t ap_setprocattr_io(ap_procattr_t *attr, ap_int32_t in, 
 *                               ap_int32_t *out, ap_int32_t err)
 *    Determine if any of stdin, stdout, or stderr should be linked
 *    to pipes when starting a child process. 
 * arg 1) The procattr we care about. 
 * arg 2) Should stdin be a pipe bnack to the parent?
 * arg 3) Should stdout be a pipe bnack to the parent?
 * arg 4) Should stderr be a pipe bnack to the parent?
 */
ap_status_t ap_setprocattr_io(ap_procattr_t *attr, ap_int32_t in, 
                              ap_int32_t out, ap_int32_t err);

/* ***APRDOC********************************************************
 * ap_status_t ap_setprocattr_childin(ap_procattr_t *attr, ap_file_t *child_in,
 *                                    ap_file_t *parent_in)
 *    Set the child_in and/or parent_in values to existing ap_file_t
 *    values. This is NOT a required initializer function. This is
 *    useful if you have already opened a pipe (or multiple files)
 *    that you wish to use, perhaps persistently across mutiple
 *    process invocations - such as a log file. You can save some 
 *    extra function calls by not creating your own pipe since this
 *    creates one in the process space for you.
 * arg 1) The procattr we care about. 
 * arg 2) ap_file_t value to use as child_in. Must be a valid file.
 * arg 3) ap_file_t value to use as parent_in. Must be a valid file.
 */
ap_status_t ap_setprocattr_childin(struct ap_procattr_t *attr, ap_file_t *child_in,
                                   ap_file_t *parent_in);

/* ***APRDOC********************************************************
 * ap_status_t ap_setprocattr_childout(ap_procattr_t *attr, 
 *                                     ap_file_t *child_out, 
 *                                     ap_file_t *parent_out)
 *    Set the child_out and parent_out values to existing ap_file_t
 *    values. This is NOT a required initializer function. This is
 *    useful if you have already opened a pipe (or multiple files)
 *    that you wish to use, perhaps persistently across mutiple
 *    process invocations - such as a log file. 
 * arg 1) The procattr we care about. 
 * arg 2) ap_file_t value to use as child_out. Must be a valid file.
 * arg 3) ap_file_t value to use as parent_out. Must be a valid file.
 */
ap_status_t ap_setprocattr_childout(struct ap_procattr_t *attr, 
                                    ap_file_t *child_out, 
                                    ap_file_t *parent_out);

/* ***APRDOC********************************************************
 * ap_status_t ap_setprocattr_childerr(ap_procattr_t *attr, 
 *                                     ap_file_t *child_err,
 *                                     ap_file_t *parent_err)
 *    Set the child_err and parent_err values to existing ap_file_t
 *    values. This is NOT a required initializer function. This is
 *    useful if you have already opened a pipe (or multiple files)
 *    that you wish to use, perhaps persistently across mutiple
 *    process invocations - such as a log file. 
 * arg 1) The procattr we care about. 
 * arg 2) ap_file_t value to use as child_err. Must be a valid file.
 * arg 3) ap_file_t value to use as parent_err. Must be a valid file.
 */
ap_status_t ap_setprocattr_childerr(struct ap_procattr_t *attr, 
                                    ap_file_t *child_err,
                                    ap_file_t *parent_err);

/* ***APRDOC********************************************************
 * ap_status_t ap_setprocattr_dir(ap_procattr_t *attr, constchar *dir) 
 *    Set which directory the child process should start executing in. 
 * arg 1) The procattr we care about. 
 * arg 2) Which dir to start in.  By default, this is the same dir as
 *        the parent currently resides in, when the createprocess call
 *        is made. 
 */
ap_status_t ap_setprocattr_dir(ap_procattr_t *attr, const char *dir);

/* ***APRDOC********************************************************
 * ap_status_t ap_setprocattr_cmdtype(ap_procattr_t *attr, ap_cmdtype_e cmd) 
 *    Set what type of command the child process will call. 
 * arg 1) The procattr we care about. 
 * arg 2) The type of command.  One of:
 *            APR_SHELLCMD --  Shell script
 *            APR_PROGRAM  --  Executable program   (default) 
 */
ap_status_t ap_setprocattr_cmdtype(ap_procattr_t *attr, ap_cmdtype_e cmd);

/* ***APRDOC********************************************************
 * ap_status_t ap_setprocattr_detach(ap_procattr_t *attr, ap_int32_t detach) 
 *    Determine if the chlid should start in detached state.
 * arg 1) The procattr we care about. 
 * arg 2) Should the child start in detached state?  Default is no. 
 */
ap_status_t ap_setprocattr_detach(ap_procattr_t *attr, ap_int32_t detach);

/* ***APRDOC********************************************************
 * ap_status_t ap_get_procdata(char *key, void *data, ap_proc_t *proc)
 *    Return the context associated with the current proc.
 * arg 1) The key associated with the data to retreive.
 * arg 2) The user data associated with the proc.
 * arg 3) The currently open proc.
 */
ap_status_t ap_get_procdata(char *key, void *data, ap_proc_t *proc);

/* ***APRDOC********************************************************
 * ap_status_t ap_set_procdata(void *data, char *key,
 *                             ap_status_t (*cleanup) (void *), ap_proc_t *proc)
 *    Return the context associated with the current proc.
 * arg 2) The user data to associate with the file.
 * arg 3) The key to use for assocaiteing data with the file.
 * arg 4) The cleanup routine to use when the file is destroyed.
 * arg 1) The current process.
 */
ap_status_t ap_set_procdata(void *data, char *key,
                            ap_status_t (*cleanup) (void *), ap_proc_t *proc);

/* ***APRDOC********************************************************
 * ap_status_t ap_get_childin(ap_file_t **new, ap_proc_t *proc) 
 *    Get the file handle that is assocaited with a child's stdin.
 * arg 1) The returned file handle. 
 * arg 2) The process handle that corresponds to the desired child process 
 */
ap_status_t ap_get_childin(ap_file_t **new, ap_proc_t *proc);

/* ***APRDOC********************************************************
 * ap_status_t ap_get_childout(ap_file_t **new, ap_proc_t *proc) 
 *    Get the file handle that is assocaited with a child's stdout.
 * arg 1) The returned file handle. 
 * arg 2) The process handle that corresponds to the desired child process 
 */
ap_status_t ap_get_childout(ap_file_t **new, ap_proc_t *proc);

/* ***APRDOC********************************************************
 * ap_status_t ap_get_childerr(ap_file_t **new, ap_proc_t *proc) 
 *    Get the file handle that is assocaited with a child's stderr.
 * arg 1) The returned file handle. 
 * arg 2) The process handle that corresponds to the desired child process 
 */
ap_status_t ap_get_childerr(ap_file_t **new, ap_proc_t *proc);

/* ***APRDOC********************************************************
 * ap_status_t ap_fork(ap_proc_t **proc, ap_context_t *cont) 
 *    This is currently the only non-portable call in APR.  This executes
 *    a standard unix fork.
 * arg 1) The resulting process handle. 
 * arg 2) The context to use. 
 */
ap_status_t ap_fork(ap_proc_t **proc, ap_context_t *cont);

/* ***APRDOC********************************************************
 * ap_status_t ap_create_process(ap_proc_t **new, const char *progname,
 *                               char *const args[], char **env, 
 *                               ap_procattr_t *attr, ap_context_t *cont) 
 *    Create a new process and execute a new program within that process.
 * arg 1) The resulting process handle.
 * arg 2) The program to run 
 * arg 3) the arguments to pass to the new program.  The first one should
 *        be the program name.
 * arg 4) The new environment ap_table_t for the new process.  This should be a
 *        list of NULL-terminated strings.
 * arg 5) the procattr we should use to determine how to create the new
 *        process
 * arg 6) The context to use. 
 */
ap_status_t ap_create_process(ap_proc_t **new, const char *progname, 
                              char *const args[], char **env, 
                              ap_procattr_t *attr, ap_context_t *cont);

/* ***APRDOC********************************************************
 * ap_status_t ap_wait_proc(ap_proc_t *proc, ap_wait_how waithow) 
 *    Wait for a child process to die 
 * arg 1) The process handle that corresponds to the desired child process 
 * arg 2) How should we wait.  One of:
 *            APR_WAIT   -- block until the child process dies.
 *            APR_NOWAIT -- return immediately regardless of if the 
 *                          child is dead or not.
 * NOTE:  The childs status is in the return code to this process.  It is 
 *        one of:
 *            APR_CHILD_DONE     -- child is no longer running.
 *            APR_CHILD_NOTDONE  -- child is still running.
 */
ap_status_t ap_wait_proc(ap_proc_t *proc, ap_wait_how_e waithow);

/* ***APRDOC********************************************************
 * ap_status_t ap_detach(ap_proc_t **new, ap_context_t *cont)
 *    Detach the process from the controlling terminal.
 * arg 1) The new process handler
 * arg 2) The context to use if it is needed.
 */
ap_status_t ap_detach(ap_proc_t **new, ap_context_t *cont);

/* ***APRDOC********************************************************
 * void ap_register_other_child(ap_proc_t *pid, 
 *                    void (*maintenance) (int reason, void *data),
 *                    void *data, int write_fd, ap_context_t *p)
 *    Register an other_child -- a child which must be kept track of so
 *    that the program knows when it has dies or disappeared.
 * arg 1)  pid is the pid of the child.
 * arg 2)  maintenance is a function that is invoked with a reason and the
 *         data pointer passed here.
 * arg 3)  The data to pass to the maintenance function.
 * arg 4)  An fd that is probed for writing.  If it is ever unwritable
 *         then the maintenance is invoked with reason OC_REASON_UNWRITABLE.
 * arg 5)  The context to use for allocating memory.
 */
void ap_register_other_child(ap_proc_t *pid, 
                             void (*maintenance) (int reason, void *),
                             void *data, int write_fd, ap_context_t *p);

/* ***APRDOC********************************************************
 * void ap_unregister_other_child(void *data)
 *    Stop watching the specified process.
 * arg 1) The data to pass to the maintenance function.  This is
 *        used to find the process to unregister.
 * NOTE:  Since this can be called by a maintenance function while we're
 *        scanning the other_children list, all scanners should protect 
 *        themself by loading ocr->next before calling any maintenance 
 *        function.
 */
void ap_unregister_other_children(void *data);

/* ***APRDOC********************************************************
 * ap_status_t ap_reap_other_child(ap_proc_t *piod)
 *    Check on the specified process.  If it is gone, call the
 *    maintenance function.
 * arg 1) The process to check.
 */
ap_status_t ap_reap_other_child(ap_proc_t *pid); 

/* ***APRDOC********************************************************
 * void ap_check_other_child(void)
 *    Loop through all registered other_children and call the
 *    appropriate maintenance function when necessary.
 */
void ap_check_other_child(void); 

/* ***APRDOC********************************************************
 * ap_status_t ap_kill(ap_proc_t *proc, int sig)
 *    Terminate a process.
 * arg 1) The process to terminate.
 * arg 2) How to kill the process.
 */
ap_status_t ap_kill(ap_proc_t *proc, int sig);
#ifdef __cplusplus
}
#endif

#endif  /* ! APR_FILE_IO_H */








