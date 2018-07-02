#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QFormLayout>
#include <QButtonGroup>
#include <QCheckBox>
#include <QDebug>
#include <QHeaderView>

#include "sp_search_dialog.h"

search_dialog::search_dialog(QWidget* parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    SPDBManager    = 0;
    SearchButton   = new QPushButton("Search");
    ResultsView    = new QTreeWidget();
    SearchTerms    = new QLineEdit();
    ResultsCounter = new QLabel();
    ResultsCounter->hide();

    ResultsView->setSortingEnabled(true);
    ResultsView->setColumnCount(2);
    ResultsView->setHeaderLabels(QStringList() << "Title" << "Occurences" << "Status");
    QHeaderView* Header = ResultsView->header();
    Header->resizeSection(0, 175);

    QVBoxLayout* OverallLayout    = new QVBoxLayout(this);
    QFormLayout* SearchFormLayout = new QFormLayout();

    QGroupBox* FilterBox         = new QGroupBox("Filters");
    QHBoxLayout* FilterBoxLayout = new QHBoxLayout(FilterBox);

    QCheckBox* Notes             = new QCheckBox("Notes");
    QCheckBox* Claims            = new QCheckBox("Claims");
    QCheckBox* LogEntries        = new QCheckBox("Log Entries");
    QCheckBox* ScratchEntries    = new QCheckBox("Scratchpad Entries");

    FilterBoxLayout->addWidget(Notes);
    FilterBoxLayout->addWidget(Claims);
    FilterBoxLayout->addWidget(LogEntries);
    FilterBoxLayout->addWidget(ScratchEntries);

    SearchFormLayout->addRow("Search for:", SearchTerms);
    SearchFormLayout->addRow(FilterBox);
    SearchFormLayout->addRow(SearchButton);

    OverallLayout->addLayout(SearchFormLayout);

    QGroupBox* ResultsBox      = new QGroupBox("Search Results");
    QVBoxLayout* ResultsLayout = new QVBoxLayout();
    ResultsLayout->addWidget(ResultsCounter);
    ResultsLayout->addWidget(ResultsView);
    ResultsBox->setLayout(ResultsLayout);

    OverallLayout->addWidget(ResultsBox);

    connect(SearchButton, &QPushButton::pressed, this, &search_dialog::BeginSearch);
    connect(this, &search_dialog::finished, this, &search_dialog::ResetDialog);
}

search_dialog::~search_dialog()
{
    delete SearchButton;
    delete ResultsView;
    delete SearchTerms;
}

struct SearchResult {
    QDateTime Time;
    QString   StateHash;
    int       Frequency;
};

void
search_dialog::SetSPDBManager(spdb_manager* NewManager)
{
    ResetDialog();
    SPDBManager = NewManager;
}

void
search_dialog::BeginSearch()
{
    ResultsView->clear();
    QString searchText = SearchTerms->text();
    NotYetImplemented;

}

void
search_dialog::ResetDialog()
{
    ResultsView->clear();
    ResultsCounter->hide();
    SearchTerms->clear();
}
