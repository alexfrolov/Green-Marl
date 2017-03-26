



/* ---------------- method closures -------------- */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_Main::do_color_scc_2_closure : public SDAG::Closure {
      

      do_color_scc_2_closure() {
        init();
      }
      do_color_scc_2_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~do_color_scc_2_closure() {
      }
      PUPable_decl(SINGLE_ARG(do_color_scc_2_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_Main::done_3_closure : public SDAG::Closure {
      

      done_3_closure() {
        init();
      }
      done_3_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~done_3_closure() {
      }
      PUPable_decl(SINGLE_ARG(done_3_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_Main::exit_4_closure : public SDAG::Closure {
      

      exit_4_closure() {
        init();
      }
      exit_4_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~exit_4_closure() {
      }
      PUPable_decl(SINGLE_ARG(exit_4_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_Main::sccCounter_inc_5_closure : public SDAG::Closure {
      

      sccCounter_inc_5_closure() {
        init();
      }
      sccCounter_inc_5_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~sccCounter_inc_5_closure() {
      }
      PUPable_decl(SINGLE_ARG(sccCounter_inc_5_closure));
    };
#endif /* CK_TEMPLATES_ONLY */






/* DEFS: readonly CProxy_Main main_proxy;
 */
extern CProxy_Main main_proxy;
#ifndef CK_TEMPLATES_ONLY
extern "C" void __xlater_roPup_main_proxy(void *_impl_pup_er) {
  PUP::er &_impl_p=*(PUP::er *)_impl_pup_er;
  _impl_p|main_proxy;
}
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: readonly CmiUInt8 N;
 */
extern CmiUInt8 N;
#ifndef CK_TEMPLATES_ONLY
extern "C" void __xlater_roPup_N(void *_impl_pup_er) {
  PUP::er &_impl_p=*(PUP::er *)_impl_pup_er;
  _impl_p|N;
}
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: mainchare Main: Chare{
Main(CkArgMsg* impl_msg);
void do_color_scc();
void done();
void exit();
void sccCounter_inc();
};
 */
#ifndef CK_TEMPLATES_ONLY
 int CkIndex_Main::__idx=0;
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
/* DEFS: Main(CkArgMsg* impl_msg);
 */

CkChareID CProxy_Main::ckNew(CkArgMsg* impl_msg, int impl_onPE)
{
  CkChareID impl_ret;
  CkCreateChare(CkIndex_Main::__idx, CkIndex_Main::idx_Main_CkArgMsg(), impl_msg, &impl_ret, impl_onPE);
  return impl_ret;
}

void CProxy_Main::ckNew(CkArgMsg* impl_msg, CkChareID* pcid, int impl_onPE)
{
  CkCreateChare(CkIndex_Main::__idx, CkIndex_Main::idx_Main_CkArgMsg(), impl_msg, pcid, impl_onPE);
}

  CProxy_Main::CProxy_Main(CkArgMsg* impl_msg, int impl_onPE)
{
  CkChareID impl_ret;
  CkCreateChare(CkIndex_Main::__idx, CkIndex_Main::idx_Main_CkArgMsg(), impl_msg, &impl_ret, impl_onPE);
  ckSetChareID(impl_ret);
}

// Entry point registration function

int CkIndex_Main::reg_Main_CkArgMsg() {
  int epidx = CkRegisterEp("Main(CkArgMsg* impl_msg)",
      _call_Main_CkArgMsg, CMessage_CkArgMsg::__idx, __idx, 0);
  CkRegisterMessagePupFn(epidx, (CkMessagePupFn)CkArgMsg::ckDebugPup);
  return epidx;
}


void CkIndex_Main::_call_Main_CkArgMsg(void* impl_msg, void* impl_obj_void)
{
  Main* impl_obj = static_cast<Main *>(impl_obj_void);
  new (impl_obj) Main((CkArgMsg*)impl_msg);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void do_color_scc();
 */

void CProxy_Main::do_color_scc(const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
    int destPE=CkChareMsgPrep(CkIndex_Main::idx_do_color_scc_void(), impl_msg, &ckGetChareID());
    if (destPE!=-1) ckDelegatedTo()->ChareSend(ckDelegatedPtr(),CkIndex_Main::idx_do_color_scc_void(), impl_msg, &ckGetChareID(),destPE);
  }
  else CkSendMsg(CkIndex_Main::idx_do_color_scc_void(), impl_msg, &ckGetChareID(),0);
}

// Entry point registration function

int CkIndex_Main::reg_do_color_scc_void() {
  int epidx = CkRegisterEp("do_color_scc()",
      _call_do_color_scc_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_Main::_call_do_color_scc_void(void* impl_msg, void* impl_obj_void)
{
  Main* impl_obj = static_cast<Main *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->do_color_scc();
}
PUPable_def(SINGLE_ARG(Closure_Main::do_color_scc_2_closure))
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void done();
 */

void CProxy_Main::done(const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
    int destPE=CkChareMsgPrep(CkIndex_Main::idx_done_void(), impl_msg, &ckGetChareID());
    if (destPE!=-1) ckDelegatedTo()->ChareSend(ckDelegatedPtr(),CkIndex_Main::idx_done_void(), impl_msg, &ckGetChareID(),destPE);
  }
  else CkSendMsg(CkIndex_Main::idx_done_void(), impl_msg, &ckGetChareID(),0);
}

// Entry point registration function

int CkIndex_Main::reg_done_void() {
  int epidx = CkRegisterEp("done()",
      _call_done_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_Main::_call_done_void(void* impl_msg, void* impl_obj_void)
{
  Main* impl_obj = static_cast<Main *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->done();
}
PUPable_def(SINGLE_ARG(Closure_Main::done_3_closure))
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void exit();
 */

void CProxy_Main::exit(const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
    int destPE=CkChareMsgPrep(CkIndex_Main::idx_exit_void(), impl_msg, &ckGetChareID());
    if (destPE!=-1) ckDelegatedTo()->ChareSend(ckDelegatedPtr(),CkIndex_Main::idx_exit_void(), impl_msg, &ckGetChareID(),destPE);
  }
  else CkSendMsg(CkIndex_Main::idx_exit_void(), impl_msg, &ckGetChareID(),0);
}

// Entry point registration function

int CkIndex_Main::reg_exit_void() {
  int epidx = CkRegisterEp("exit()",
      _call_exit_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_Main::_call_exit_void(void* impl_msg, void* impl_obj_void)
{
  Main* impl_obj = static_cast<Main *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->exit();
}
PUPable_def(SINGLE_ARG(Closure_Main::exit_4_closure))
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void sccCounter_inc();
 */

void CProxy_Main::sccCounter_inc(const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
    int destPE=CkChareMsgPrep(CkIndex_Main::idx_sccCounter_inc_void(), impl_msg, &ckGetChareID());
    if (destPE!=-1) ckDelegatedTo()->ChareSend(ckDelegatedPtr(),CkIndex_Main::idx_sccCounter_inc_void(), impl_msg, &ckGetChareID(),destPE);
  }
  else CkSendMsg(CkIndex_Main::idx_sccCounter_inc_void(), impl_msg, &ckGetChareID(),0);
}

// Entry point registration function

int CkIndex_Main::reg_sccCounter_inc_void() {
  int epidx = CkRegisterEp("sccCounter_inc()",
      _call_sccCounter_inc_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_Main::_call_sccCounter_inc_void(void* impl_msg, void* impl_obj_void)
{
  Main* impl_obj = static_cast<Main *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->sccCounter_inc();
}
PUPable_def(SINGLE_ARG(Closure_Main::sccCounter_inc_5_closure))
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
void CkIndex_Main::__register(const char *s, size_t size) {
  __idx = CkRegisterChare(s, size, TypeMainChare);
  CkRegisterBase(__idx, CkIndex_Chare::__idx);
  // REG: Main(CkArgMsg* impl_msg);
  idx_Main_CkArgMsg();
  CkRegisterMainChare(__idx, idx_Main_CkArgMsg());

  // REG: void do_color_scc();
  idx_do_color_scc_void();

  // REG: void done();
  idx_done_void();

  // REG: void exit();
  idx_exit_void();

  // REG: void sccCounter_inc();
  idx_sccCounter_inc_void();

}
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group graphlib::rmat_generator<color_scc_graph >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group graphlib::random_generator<color_scc_graph >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
void _registercolor_scc_main(void)
{
  static int _done = 0; if(_done) return; _done = 1;
  _registercolor_scc();

  _registergm_graph();

  CkRegisterReadonly("main_proxy","CProxy_Main",sizeof(main_proxy),(void *) &main_proxy,__xlater_roPup_main_proxy);

  CkRegisterReadonly("N","CmiUInt8",sizeof(N),(void *) &N,__xlater_roPup_N);

/* REG: mainchare Main: Chare{
Main(CkArgMsg* impl_msg);
void do_color_scc();
void done();
void exit();
void sccCounter_inc();
};
*/
  CkIndex_Main::__register("Main", sizeof(Main));

/* REG: group graphlib::rmat_generator<color_scc_graph >: IrrGroup;
*/
  graphlib::CkIndex_rmat_generator<color_scc_graph >::__register("graphlib::rmat_generator<color_scc_graph >", sizeof(graphlib::rmat_generator<color_scc_graph >));

/* REG: group graphlib::random_generator<color_scc_graph >: IrrGroup;
*/
  graphlib::CkIndex_random_generator<color_scc_graph >::__register("graphlib::random_generator<color_scc_graph >", sizeof(graphlib::random_generator<color_scc_graph >));

}
extern "C" void CkRegisterMainModule(void) {
  _registercolor_scc_main();
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void CBase_Main::virtual_pup(PUP::er &p) {
    recursive_pup<Main >(dynamic_cast<Main* >(this), p);
}
#endif /* CK_TEMPLATES_ONLY */
