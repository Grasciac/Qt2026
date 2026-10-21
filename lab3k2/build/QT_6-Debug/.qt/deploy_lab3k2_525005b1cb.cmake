include("C:/Users/gegee/OneDrive/Документы/lab3k2/build/QT_6-Debug/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lab3k2-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE "C:/Users/gegee/OneDrive/Документы/lab3k2/build/QT_6-Debug/lab3k2.exe"
    GENERATE_QT_CONF
)
