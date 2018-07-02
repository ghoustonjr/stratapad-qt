#include <QtGui>
#include <QtWidgets>
#include <QCommandLinkButton>

#include "sp_tag_dialog.h"
#include "sp_node_editor.h"

sp_node_editor::sp_node_editor(QWidget *parent) : QWidget(parent)
{
    setObjectName("NodeEditor");
    MainLayout = new QVBoxLayout(this);
    MainLayout->setMargin(0);
    MainLayout->setSpacing(0);

    Title = new QLabel(this);
    Title->setText("Document Title");
    Title->setObjectName("editorDocTitle");
    Title->setContentsMargins(5,5,5,5);
    Title->setWordWrap(true);

    Editor = new sp_richtext_editor(this);
    Links = new QTableView(this);
    ConstraintsModel = new sp_constraints_model(this);
    Links->setModel(ConstraintsModel);
    Links->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    Links->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    Links->horizontalHeader()->setStretchLastSection(true);
    Links->resizeColumnToContents(0);
    Links->verticalHeader()->hide();

    QSplitter *Splitter = new QSplitter(this);
    Splitter->setOrientation(Qt::Vertical);
    Splitter->addWidget(Editor);
    Splitter->addWidget(Links);

    MainLayout->addWidget(Title);
    SetupTags(QStringList());
    MainLayout->addWidget(Splitter);

    Manager = 0;
}

void
sp_node_editor::SetSPDBManager(spdb_manager *NewManager)
{
    Manager = NewManager;
    connect(&Manager->Signaler, &spdb_signaler::TxComplete, this, &sp_node_editor::ProcessTxReport);
}

void
sp_node_editor::SetNode(const QUuid &Id, const QString &BasisHash)
{
    NodeId = Id;
    NodeBasisHash = BasisHash;

    node Node = {};
    status_code Result = NodeState(Manager, BasisHash, &Node);
    Q_ASSERT(Result == StatusCode_OK);

    Title->setText(SpConv(Node.Title));

    QStringList Tags;
    tag_link *UpdatedTags = Sp_HeadCommitTags(Manager->Repo);
    FOR_EACH(tag_link*, Link, UpdatedTags)
    {
        QUuid TagId = SpConv(Link->Id);
        if(Id == TagId)
        {
            Tags << SpConv(Link->Text);
        }
    }

    SetupTags(Tags);
}

void
sp_node_editor::SetupTags(const QStringList &Tags)
{
    //TODO: Take a look at this. Can it fail?
    QLayoutItem *LayoutItem = MainLayout->takeAt(1);
    if(LayoutItem)
    {
        delete LayoutItem->widget();
    }

    QWidget *TagsWidget = new QWidget(this);
    TagsWidget->setObjectName("NodeTags");

    sp_flow_layout *Layout = new sp_flow_layout(NULL, 5, 5, 5);
    TagsWidget->setLayout(Layout);

    for(QString String : Tags)
    {
        QLabel *Tag = new QLabel();
        Tag->setText(String);
        Tag->setObjectName("tag");
        Tag->setContentsMargins(7, 2, 7, 2);
        Layout->addWidget(Tag);
    }

    QLabel *AddTagLink = new QLabel;
    AddTagLink->setContentsMargins(2, 3, 2, 2);
    AddTagLink->setText("<a href=\"none\">Add tag...</a>");
    AddTagLink->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);

    connect(AddTagLink, &QLabel::linkActivated, this, &sp_node_editor::ShowNewTagDialog);

    Layout->addWidget(AddTagLink);
    MainLayout->insertWidget(1, TagsWidget);
}

void
sp_node_editor::ProcessTxReport(tx_report *TxReport)
{
    qDebug() << "New TX Report";
    FOR_EACH(tx_report_link*, Link, &TxReport->Updated)
    {
        QUuid LinkId = SpConv(Link->Id);
        if(LinkId == NodeId)
        {
            NodeBasisHash = SpConv(Link->Hash);

            node Node = {};
            status_code RC = NodeState(Manager, NodeBasisHash, &Node);
            Q_ASSERT(RC == StatusCode_OK);

            Title->setText(SpConv(Node.Title));
        }
    }

    FOR_EACH(uuid_link*, Link, &TxReport->UpdatedTags)
    {
        QUuid UpdatedId = SpConv(Link->Id);
        if(UpdatedId == NodeId)
        {
            qDebug() << "Node has updated tags!";
            QStringList NewTags;

            tag_link *UpdatedTags = Sp_HeadCommitTags(Manager->Repo);
            FOR_EACH(tag_link*, Link, UpdatedTags)
            {
                QUuid TagId = SpConv(Link->Id);
                if(UpdatedId == TagId)
                {
                    NewTags << SpConv(Link->Text);
                }
            }

            SetupTags(NewTags);
        }
    }
}

void
sp_node_editor::ShowNewTagDialog()
{
    sp_tag_dialog d;
    connect(&d, &sp_tag_dialog::TagCreated,
           [=]
           (const QString &Tag)
           {
               status_code RC = AddTag(Manager, NodeId, Tag);
               Q_ASSERT(RC == StatusCode_OK);
           });
    d.exec();
}
