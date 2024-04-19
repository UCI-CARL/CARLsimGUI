#ifndef DESCRIPTIONDIALOG_H
#define DESCRIPTIONDIALOG_H

// SpikeStream 0.3
#include "api.h"

//Qt includes
#include <QDialog>
#include <QLineEdit>
#include <QPlainTextEdit>

namespace spikestream {

	/*! Dialog for editing a description. */
	class SPIKESTREAM_APP_LIB_EXPORT DescriptionDialog : public QDialog {
		Q_OBJECT

		public:
			DescriptionDialog(const QString& description, QWidget* parent=0);
			DescriptionDialog(const QString& description, bool multiline, QWidget* parent = 0);
			~DescriptionDialog();
			QString getDescription();


		private slots:
			void okButtonPressed();
			void cancelButtonPressed();


		private:
			//====================  VARIABLES ====================
			/*! Where user enters a description. */
			QLineEdit* descLineEdit;
			QPlainTextEdit* descTextEdit;

			//====================  METHODS  ====================
			void buildGUI(const QString& description);
			void buildMultilineGUI(const QString& description);
	};
}

#endif//DESCRIPTIONDIALOG_H

