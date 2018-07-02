#include <QCryptographicHash>
#include <QTextCodec>
#include <QDateTime>
#include <QSaveFile>
#include <QDebug>
#include <QTextDocument>
#include <QPair>
#include <QScopedPointer>
#include <QFileInfo>

#include "sp_db_manager.h"
#include "sp_util.h"

PLATFORM_GET_TIME(QtGetTime)
{
    return QDateTime::currentSecsSinceEpoch();
}

#include <Windows.h>

PLATFORM_GET_MEMORY(Win32GetMemory)
{
    platform_memory_block *Result = 0;

    size_t TotalSize = sizeof(platform_memory_block) + Size;
    Result = (platform_memory_block *) VirtualAlloc(BaseAddress, TotalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    DWORD Err = GetLastError();

    Result->Size = Size;
    Result->Used = 0;
    Result->Base = ((u8*)Result + sizeof(platform_memory_block));

    LPWSTR ErrMsg = 0;
    DWORD FmtRes = FormatMessageW(
      _In_     FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      _In_opt_ 0,
      _In_     Err,
      _In_     LANG_SYSTEM_DEFAULT,
      _Out_    ErrMsg,
      _In_     256,
      _In_opt_ 0
    );

    Post(Result);
    return(Result);
}

PLATFORM_FREE_MEMORY(Win32FreeMemory)
{
    BOOL Result = VirtualFree((void*)Block->Base, 0, MEM_RELEASE);
    if(!Result)
    {
        LPWSTR ErrMsg = 0;
        DWORD Err = GetLastError();
        DWORD FmtRes = FormatMessageW(
                _In_     FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                _In_opt_ 0,
                _In_     Err,
                _In_     LANG_SYSTEM_DEFAULT,
                _Out_    ErrMsg,
                _In_     256,
                _In_opt_ 0
                );
        InvalidCodePath;
    }
}

void
InitBackend()
{
    platform_api Api = {};
    Api.GetTime = &QtGetTime;
    Api.AllocateMemory = &Win32GetMemory;
    Api.FreeMemory = &Win32FreeMemory;

    Sp_LibraryInit(Api);
}

status_code
Transact(spdb_manager *Manager)
{
    Sp_Transact(Manager->Repo);
    tx_report *Report;
    Sp_HeadTxReport(Manager->Repo, &Report);
    emit Manager->Signaler.TxComplete(Report);

    return(StatusCode_OK);
}

spdb_manager*
OpenRepo(const QFile &RepoFile)
{
    Q_ASSERT(RepoFile.exists());

    spdb_manager *Result = 0;

    QFileInfo RepoFileInfo(RepoFile);
    QByteArray RepoPathBytes = RepoFileInfo.absoluteFilePath().toLocal8Bit();

    repository *SpRepo = Sp_OpenRepository(RepoPathBytes.constData());
    Q_ASSERT(SpRepo);

    Result = new spdb_manager;
    Result->DbFilePath = RepoFileInfo.absoluteFilePath();
    Result->Repo = SpRepo;

    return(Result);
}

spdb_manager*
CreateRepo(const QFile &RepoFileName)
{
    Q_ASSERT(!RepoFileName.exists());

    spdb_manager *Result = new spdb_manager;

    QFileInfo RepoFileInfo(RepoFileName);
    QByteArray RepoFilePathBytes = RepoFileInfo.absoluteFilePath().toUtf8();
    const char *FinalPath = RepoFilePathBytes.data();

    repository *SpRepo = Sp_CreateRepository(FinalPath);
    Q_ASSERT(SpRepo);

    Result->DbFilePath = RepoFileInfo.absoluteFilePath();
    Result->Repo = SpRepo;

    return(Result);
}

status_code
NodeState(spdb_manager *Manager, const QString& NodeStateSha, node *Out)
{
    Q_ASSERT(Manager);
    Q_ASSERT(!NodeStateSha.isNull());
    Q_ASSERT(Out);

    u32 RC;
    RC = Sp_NodeState(Manager->Repo, ConvertToSpHash(NodeStateSha), Out);
    Q_ASSERT(!RC);

    status_code Result = StatusCode_OK;
    return(Result);
}

status_code
HeadCommitTimestamp(spdb_manager *Manager, s64 *Out)
{
    Q_ASSERT(Manager);
    Q_ASSERT(Out);

    Sp_HeadCommitTimestamp(Manager->Repo, Out);

    return(StatusCode_OK);
}

status_code
MakeNode(spdb_manager *Manager, basic_node NodeData)
{
    Q_ASSERT(Manager);
    Q_ASSERT(!NodeData.Title.isNull());

    status_code Result = StatusCode_ERROR;

    node Node = {};
    Node.Id = ConvertToSpUuid(QUuid::createUuid());

    QByteArray Utf8Bytes = NodeData.Title.toUtf8();
    string ConvertedTitle = {};
    ConvertedTitle.Text = (u8*)Utf8Bytes.data();
    ConvertedTitle.ByteSize = Utf8Bytes.size();

    Node.Title = ConvertedTitle;

    Sp_TxAdd(Manager->Repo, Node);
    Result = Transact(Manager);

    return(Result);
}

status_code
CommitWIP(spdb_manager *Manager, const QMap<QString, QString> &WIP)
{
    status_code Result = StatusCode_ERROR;

    for(QString BasisHash : WIP.keys())
    {
        node CurrentNode = {};
        NodeState(Manager, BasisHash, &CurrentNode);

        QString Content = WIP.value(BasisHash);
        Q_ASSERT(!Content.isNull());
        QByteArray Utf8Bytes = Content.toUtf8();

        string ConvertedContent = {};
        ConvertedContent.Text = (u8*)Utf8Bytes.data();
        ConvertedContent.ByteSize = Utf8Bytes.size();

        CurrentNode.Content = ConvertedContent;

        Sp_TxAdd(Manager->Repo, CurrentNode);
    }

    Result = Transact(Manager);
    return(Result);
}

status_code
ChangeTitle(spdb_manager *Manager, const QString &BasisHash, const QString &Title)
{
    node CurrentNode = {};
    NodeState(Manager, BasisHash, &CurrentNode);

    Q_ASSERT(!Title.isNull());
    QByteArray Utf8Bytes = Title.toUtf8();

    string ConvertedContent = {};
    ConvertedContent.Text = (u8*)Utf8Bytes.data();
    ConvertedContent.ByteSize = Utf8Bytes.size();

    CurrentNode.Title = ConvertedContent;

    Sp_TxAdd(Manager->Repo, CurrentNode);

    return(Transact(Manager));
}

status_code
DeleteNode(spdb_manager *Manager, const QUuid &noteID)
{
    status_code Result = StatusCode_ERROR;
    NotYetImplemented;
    return(Result);
}

status_code
AddTag(spdb_manager *Manager, const QUuid &Id, const QString &Tag)
{
    Q_ASSERT(Manager);
    Q_ASSERT(!Id.isNull());
    Q_ASSERT(!Tag.isNull());

    QByteArray Utf8Bytes = Tag.toUtf8();

    string ConvertedTag = {};
    ConvertedTag.Text = (u8*)Utf8Bytes.data();
    ConvertedTag.ByteSize = Utf8Bytes.size();

    Sp_TxAddTag(Manager->Repo, ConvertedTag, ConvertToSpUuid(Id));
    return(Transact(Manager));
}
