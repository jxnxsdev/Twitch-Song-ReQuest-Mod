#include "custom-types/shared/macros.hpp"
#include "bsml/shared/macros.hpp"
#include "bsml/shared/BSML.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/GameObject.hpp"

DECLARE_CLASS_CODEGEN(TSRQ, FloatingMenu, UnityEngine::MonoBehaviour,

                      DECLARE_INSTANCE_FIELD(bool, initialized);
                              DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, menu);

                              public:
                              void Initialize();
                              static void delete_instance();
                              static SafePtrUnity<TSRQ::FloatingMenu> get_instance();

private:
    static SafePtrUnity<TSRQ::FloatingMenu> instance;


)