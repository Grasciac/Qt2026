include("C:/Users/gegee/OneDrive/Документы/untitled/build/QT_6-Debug/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/untitled-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE "C:/Users/gegee/OneDrive/Документы/untitled/build/QT_6-Debug/untitled.exe"
    GENERATE_QT_CONF
)
