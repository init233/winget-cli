// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "pch.h"
#include "Invocation.h"
#include "WorkflowReporter.h"

namespace AppInstaller::Workflow
{
    using namespace std::string_view_literals;

    // Token specified in installer args will be replaced by proper value.
    static constexpr std::string_view ARG_TOKEN_LOGPATH = "<LOGPATH>"sv;
    static constexpr std::string_view ARG_TOKEN_INSTALLPATH = "<INSTALLPATH>"sv;

    // This is the base class for installer handlers. Individual installer handler should override
    // member methods to do appropriate work on different installers.
    class InstallerHandlerBase
    {
    public:

        // The Download method downloads installer to local temp folder.
        // The downloaded installer does not have any extension appended.
        // SHA256 of the downloaded installer is verified during download.
        virtual void Download();

        virtual void Install() { THROW_HR(E_NOTIMPL); }
        virtual void Cancel() { THROW_HR(E_NOTIMPL); }

    protected:

        // This will be triggered by file downloader to report download progress
        class DownloaderCallback : public AppInstaller::Utility::IDownloaderCallback
        {
        public:
            DownloaderCallback(WorkflowReporter& reporter) : m_reporterRef(reporter) {};

            void OnStarted(LONGLONG totalBytes) override;
            void OnProgress(LONGLONG bytesDownloaded, LONGLONG totalBytes) override;
            void OnCanceled() override;
            void OnCompleted() override;

        private:
            WorkflowReporter& m_reporterRef;

            // This determines if definite progress bar or indefinite progress bar should be shown.
            bool m_useProgressBar = true;
        };

        InstallerHandlerBase(
            const Manifest::ManifestInstaller& manifestInstaller,
            const CLI::Invocation& args,
            WorkflowReporter& reporter) :
            m_manifestInstallerRef(manifestInstaller), m_reporterRef(reporter), m_downloaderCallback(reporter), m_argsRef(args) {};

        const Manifest::ManifestInstaller& m_manifestInstallerRef;
        const CLI::Invocation& m_argsRef;
        WorkflowReporter& m_reporterRef;
        std::filesystem::path m_downloadedInstaller;
        DownloaderCallback m_downloaderCallback;
    };
}
