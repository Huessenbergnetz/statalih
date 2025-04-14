<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1">
<context>
    <name></name>
    <message id="stcmd-msg-done">
        <source>Done</source>
        <translation type="unfinished"></translation>
    </message>
    <message id="stcmd-msg-failed">
        <source>Failed</source>
        <translation type="unfinished"></translation>
    </message>
    <message id="stcmd-status-reading-config">
        <source>Reading configuration file</source>
        <translation type="unfinished"></translation>
    </message>
    <message id="stcmd-error-config-file-not-readable">
        <source>Can not read configuration file at %1</source>
        <extracomment>Error message, %1 will be replaced by the file path</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="stcmd-error-config-file-malformed">
        <source>Failed to parse configuration file at %1</source>
        <extracomment>Error message, %1 will be replaced by the file path</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="stcmd-status-open-db">
        <source>Establishing database connection</source>
        <translation type="unfinished"></translation>
    </message>
    <message id="stcmd-error-db-invalid-object">
        <source>Failed to obtain database object.</source>
        <extracomment>Error message</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="stcmd-error-config-invalid-dbtype">
        <source>Can not open database connection for not supported database type “%1”.</source>
        <extracomment>Error message, %1 will be the invalid DB type</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="stcmd-warn-db-set-con-tz">
        <source>Failed to set database connection time zone to UTC: %1</source>
        <extracomment>CLI warning message, %1 will be the database error</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-help-header-global-options">
        <source>Global options:</source>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-help-usage">
        <source>Usage:</source>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-help-usage-global-options">
        <source>[global options]</source>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-help-usage-command">
        <source>&lt;command&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-help-usage-options">
        <source>[options]</source>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-help-header-commands">
        <source>Commands:</source>
        <extracomment>CLI help header</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-help-header-options">
        <source>Options:</source>
        <extracomment>CLI help header</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-command-database-summary">
        <source>Manage database migrations</source>
        <extracomment>CLI command summary</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-command-database-description">
        <source>Run, roll back, reset and refresh database migrations. Be careful, these commands can easily lead to data loss! Make sure that you have made a backup of your database beforehand.</source>
        <extracomment>CLI command description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-command-database-migration-summary">
        <source>Run database migrations</source>
        <extracomment>CLI command summary</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-command-database-migration-description">
        <source>Performs all database migrations that have not already be performed before. You should run this command after upgrading Zumftmeistar.</source>
        <extracomment>CLI command description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-database-refresh-steps-desc">
        <source>Number of migration stepts to refresh. If 0, all migrations will be refreshed. Default: %1.</source>
        <extracomment>CLI option description, %1 will be replaced by the default value</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-database-refresh-steps-value">
        <source>steps</source>
        <extracomment>CLI option value name</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-error-invalid-refresh-steps-integer">
        <source>Can not perform database migration refresh. “%1” is not a valid integer value for steps.</source>
        <extracomment>Error message, %1 will be replaced by the malformed input value.</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-command-database-refresh-summary">
        <source>Refresh database migrations</source>
        <extracomment>CLI command summary</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-command-database-refresh-description">
        <source>Refreshes the specified number of migration steps. A refresh will roll back the specified number of migrations and will rerun them afterwards. Note that this will result in data loss. This command is for development purposes only.</source>
        <extracomment>CLI command description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-command-database-reset-summary">
        <source>Reset database migrations</source>
        <extracomment>CLI command summary</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-command-database-reset-description">
        <source>Resets all database migrations. A reset will roll back all migrations. Note that this will result in data loss. This command is for development purposes only.</source>
        <extracomment>CLI command description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-database-rollback-steps-desc">
        <source>Number of migration steps to roll back. Default: %1.</source>
        <extracomment>CLI option description, %1 will be replaced by the default value</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-database-rollback-steps-value">
        <source>steps</source>
        <extracomment>CLI option value name</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-error-invalid-rollback-steps-integer">
        <source>Can not perform database migration rollback. “%1” is not a valid integer value for steps.</source>
        <extracomment>CLI error message</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-command-database-rollback-summary">
        <source>Roll back database migrations</source>
        <extracomment>CLI command summary</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-command-database-rollback-description">
        <source>Rolls back the specified number of migration steps. Note that this will result in data loss. This command is for development purposes only.</source>
        <extracomment>CLI command description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-global-help-desc">
        <source>Displays help on command line options and commands.</source>
        <extracomment>CLI option description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-global-inifile-desc">
        <source>Path to configuration ini file. Default: %1</source>
        <extracomment>CLI option description, %1 will be the default path</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-global-inifile-val">
        <source>ini file</source>
        <extracomment>CLI option value name</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-global-quiet-desc">
        <source>Be quiet and print less output.</source>
        <extracomment>CLI option description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-help-description">
        <source>Zumftmeistar is a web management application for clubs and associations based on Cutelyst.
This command line client helps to manage several aspects of Zumftmeistar.</source>
        <extracomment>General description for zmc</extracomment>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
