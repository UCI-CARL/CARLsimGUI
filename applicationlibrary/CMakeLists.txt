
set(CMAKE_AUTOMOC ON)

set_property(SOURCE include/AnalysisSpectrogram.h PROPERTY SKIP_AUTOMOC ON)

add_library(carlsimguiapplication SHARED)

    set(HDRS 
        include/api.h
        include/AbstractAnalysisTimeStepThread.h
        include/AbstractAnalysisWidget.h
        include/AbstractConnectionBuilder.h
        include/AbstractConnectionWidget.h
        include/AbstractDeviceManager.h
        include/AbstractDeviceWidget.h
        include/AbstractExperimentWidget.h
        include/AbstractExportAnalysisDialog.h
        include/AbstractGraphDialog.h
        include/AbstractGraphWidget.h
        include/AbstractNeuronBuilder.h
        include/AbstractParametersEditDialog.h
        include/AbstractSimulation.h
        include/AbstractSpectrogramData.h
        include/AnalysesModel.h
        include/AnalysesTableView.h
        include/AnalysisLoaderWidget.h
        include/AnalysisParameterDialog.h
        include/AnalysisRunner.h
        include/AnalysisSpectrogram.h
        include/ArchivePlayerThread.h
        include/ArchiveWidget.h
        include/ConnectionGroupDialog.h
        include/ConnectionGroupModel.h
        include/ConnectionGroupTableView.h
        include/ConnectionManager.h
        include/ConnectionWidget.h
        include/ConnectionsModel.h
        include/ConnectionsTableView.h
        include/DatabaseManager.h
        include/DescriptionDialog.h
        include/EventRouter.h
        include/Globals.h
        include/HeatColorBar.h
        include/LoadAnalysisDialog.h
        include/MembranePotentialGraphDialog.h
        include/MembranePotentialGraphWidget.h
        include/NetworkDialog.h
        include/NetworkDisplay.h
        include/NetworkManager.h
        include/NetworkViewer.h
        include/NetworkViewerProperties.h
        include/NetworksBuilder.h
        include/NetworksWidget.h
        include/NeuronGroupDialog.h
        include/NeuronGroupModel.h
        include/NeuronGroupSelectionDialog.h
        include/NeuronGroupSelectionModel.h
        include/NeuronGroupSelectionView.h
        include/NeuronGroupTableView.h
        include/NeuronGroupWidget.h
        include/NeuronParametersDialog.h
        include/NeuronParametersEditDialog.h
        include/NeuronParametersModel.h
        include/NeuronParametersView.h
        include/ParametersDialog.h
        include/PluginManager.h
        include/PluginsDialog.h
        include/ProgressWidget.h
        include/SimulationLoaderWidget.h
        include/SpikeRasterDialog.h
        include/SpikeRasterWidget.h
        include/SpikeStreamApplication.h
        include/SpikeStreamMainWindow.h
        include/SynapseParametersDialog.h
        include/SynapseParametersEditDialog.h
        include/SynapseParametersModel.h
        include/SynapseParametersView.h
        include/TruthTableDialog.h
        include/TruthTableModel.h
        include/TruthTableView.h
        include/ViewParametersDialog.h
    )

    set(SRCS     
        src/ArchivePlayerThread.cpp
        src/Globals.cpp
        src/NetworkDisplay.cpp
        src/NetworkViewer.cpp
        src/NetworksBuilder.cpp
        src/PluginManager.cpp
        src/SpikeStreamApplication.cpp
        src/SpikeStreamMainWindow.cpp
        src/analysis/AbstractAnalysisTimeStepThread.cpp
        src/analysis/AbstractAnalysisWidget.cpp
        src/analysis/AbstractExportAnalysisDialog.cpp
        src/analysis/AbstractGraphDialog.cpp
        src/analysis/AbstractSpectrogramData.cpp
        src/analysis/AnalysisLoaderWidget.cpp
        src/analysis/AnalysisParameterDialog.cpp
        src/analysis/AnalysisRunner.cpp
        src/analysis/AnalysisSpectrogram.cpp
        src/analysis/HeatColorBar.cpp
        src/analysis/ProgressWidget.cpp
        src/builders/AbstractConnectionBuilder.cpp
        src/builders/AbstractNeuronBuilder.cpp
        src/database/DatabaseManager.cpp
        src/dialogs/AbstractParametersEditDialog.cpp
        src/dialogs/ConnectionGroupDialog.cpp
        src/dialogs/DescriptionDialog.cpp
        src/dialogs/LoadAnalysisDialog.cpp
        src/dialogs/MembranePotentialGraphDialog.cpp
        src/dialogs/NetworkDialog.cpp
        src/dialogs/NeuronGroupDialog.cpp
        src/dialogs/NeuronGroupSelectionDialog.cpp
        src/dialogs/NeuronParametersDialog.cpp
        src/dialogs/NeuronParametersEditDialog.cpp
        src/dialogs/ParametersDialog.cpp
        src/dialogs/PluginsDialog.cpp
        src/dialogs/SpikeRasterDialog.cpp
        src/dialogs/SynapseParametersDialog.cpp
        src/dialogs/SynapseParametersEditDialog.cpp
        src/dialogs/TruthTableDialog.cpp
        src/dialogs/ViewParametersDialog.cpp
        src/managers/ConnectionManager.cpp
        src/managers/NetworkManager.cpp
        src/models/AnalysesModel.cpp
        src/models/ConnectionGroupModel.cpp
        src/models/ConnectionsModel.cpp
        src/models/NeuronGroupModel.cpp
        src/models/NeuronGroupSelectionModel.cpp
        src/models/NeuronParametersModel.cpp
        src/models/SynapseParametersModel.cpp
        src/models/TruthTableModel.cpp
        src/simulation/AbstractDeviceManager.cpp
        src/simulation/AbstractDeviceWidget.cpp
        src/simulation/AbstractExperimentWidget.cpp
        src/simulation/AbstractInjectorWidget.cpp
        src/simulation/AbstractMonitorWidget.cpp
        src/simulation/AbstractSimulation.cpp
        src/views/AnalysesTableView.cpp
        src/views/ConnectionGroupTableView.cpp
        src/views/ConnectionsTableView.cpp
        src/views/NeuronGroupSelectionView.cpp
        src/views/NeuronGroupTableView.cpp
        src/views/NeuronParametersView.cpp
        src/views/SynapseParametersView.cpp
        src/views/TruthTableView.cpp
        src/widgets/AbstractConnectionWidget.cpp
        src/widgets/AbstractGraphWidget.cpp
        src/widgets/ArchiveWidget.cpp
        src/widgets/ConnectionWidget.cpp
        src/widgets/MembranePotentialGraphWidget.cpp
        src/widgets/NetworkViewerProperties.cpp
        src/widgets/NetworksWidget.cpp
        src/widgets/NeuronGroupWidget.cpp
        src/widgets/SimulationLoaderWidget.cpp
        src/widgets/SpikeRasterWidget.cpp   
    )


target_sources(carlsimguiapplication PRIVATE
        ${HDRS}
        ${SRCS}
)

add_compile_definitions(carlsimguiapplication NC_EXTENSIONS)

                                                                                                    
target_include_directories(carlsimguiapplication PRIVATE
    ../library/include
    include
#TODO Linux
#    C:\\Qwt-6.2.0\\include 
#
    /usr/include/qwt 	
)

# Since Qt 5.15, the CMake targets are also available as Qt::Core, Qt::Gui, and so on. 
# This eases writing CMake code that can work with both Qt 5 and Qt 6.
# https://doc.qt.io/qt-5/cmake-get-started.html#imported-library-targets
#
target_link_libraries(carlsimguiapplication PRIVATE
        carlsimguilib
# TODO Linux
#        C:\\Qwt-6.2.0\\lib\\qwtd.lib
# found in (base) ln@pi5:/usr/lib
	libqwt-qt5.so

        Qt::Core
        Qt::OpenGL
# TODO Linux
#        opengl32.lib
#        glu32.lib
#
# https://stackoverflow.com/questions/15459191/libgl-so-libglu-so-and-libglut-so-are-not-found-in-the-standard-path
	GL
	GLU
	glut

	gmp
	
        Qt::Gui
        Qt::PrintSupport
        Qt::Sql
        Qt::Xml
)

set_property(TARGET carlsimguiapplication PROPERTY PUBLIC_HEADER
        ${HDRS}
)

install(TARGETS carlsimguiapplication
        EXPORT carlsimguiapplication
        RUNTIME 
            DESTINATION bin 
            COMPONENT carlsimguiapplication
        LIBRARY
            DESTINATION lib
            COMPONENT carlsimguiapplication
        ARCHIVE
            DESTINATION lib
            COMPONENT carlsimguiapplication
        PUBLIC_HEADER
            DESTINATION include
            COMPONENT carlsimguiapplication
)      
    
 
