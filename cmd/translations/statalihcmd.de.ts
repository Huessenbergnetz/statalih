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
    <message id="statalihcmd-opt-global-quiet-desc">
        <source>Be quiet and print less output.</source>
        <extracomment>CLI option description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-help-description">
        <source>Statalih is a web based RSS feed reader and aggregator that can show feed content based on distance to a reference point.
This command line client helps to manage several aspects of Statalih.</source>
        <oldsource>Statalih is a Cutelyst based web based RSS feed reader and aggregator that can show feed content based on distance to a reference point.
This command line client helps to manage several aspects of Statalih.</oldsource>
        <extracomment>General description for zmc</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-command-feeds-summary">
        <source>Manage web feeds</source>
        <extracomment>CLI command summary</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-command-feeds-description">
        <source>Add, list, remove, update and fetch web feeds.</source>
        <oldsource>Add, list, remove, update and change web feeds.</oldsource>
        <extracomment>CLI command description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-feeds-add-url-desc">
        <source>URL of the web feed to add.</source>
        <extracomment>CLI option description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-err-feeds-add-missing-url">
        <source>Please use the -u parameter to specify the URL of the web feed.</source>
        <extracomment>Error message</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-err-feeds-add-invalid-url">
        <source>The web feed url is not valid.</source>
        <extracomment>Error message</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-err-feeds-add-invalid-url-scheme">
        <source>Not supported URL scheme. Only the following schemes are supported: %1</source>
        <extracomment>Error message, %1 will be replaced by a list of supported schemes</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-command-feeds-add-summary">
        <source>Add a new web feed</source>
        <extracomment>CLI command summary</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-command-feeds-add-description">
        <source>Adds a new web feed to the database. Will download the feed, parse it and store the information and items to the system.</source>
        <oldsource>Adds a new web feed to the database. Will download the feed, parse it and the information and items to the system.</oldsource>
        <extracomment>CLI command description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-status-parsing-input">
        <source>Parsing input values</source>
        <extracomment>Status message</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-err-feeds-add-invalid-coords">
        <source>Invalid coordinates</source>
        <extracomment>Error message</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statlihcmd-status-feed-add-fetch">
        <source>Fetching feed</source>
        <extracomment>Status message</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statlihcmd-err-feeds-add-parsing">
        <source>Failed to parse feed XML data at line %1 and column %2: %3</source>
        <extracomment>Error message, %1 and %2 will be replaced by line and column, %3 by the error message of the parser</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statlihcmd-status-feed-add-parse-xml">
        <source>Parsing XML</source>
        <extracomment>Satus message</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-status-feeds-add-db-add">
        <source>Adding new web feed</source>
        <oldsource>Adding new web feed “%1“</oldsource>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-err-feeds-add-already-added">
        <source>This web feed has already been added to the database with ID %1.</source>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-feeds-add-title-desc">
        <source>Overrides the web feed title extracted from the feed data.</source>
        <extracomment>CLI option description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-feeds-add-slug-desc">
        <source>Overrides the slug generated from the title extracted form the feed data.</source>
        <extracomment>CLI option description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-feeds-add-description-desc">
        <source>Overrides the web feed description extracted from the feed data.</source>
        <extracomment>CLI option description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-value-text">
        <source>text</source>
        <extracomment>CLI option value name</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-value-filepath">
        <source>file path</source>
        <extracomment>CLI option value name</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-format-desc">
        <source>Render output in a particular format. Available: %1. Default: %2.</source>
        <extracomment>CLI option description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-format-value">
        <source>format</source>
        <extracomment>CLI option value name</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-status-feeds-add-fetch-imgs">
        <source>Fetching item images</source>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalih-msg-feeds-add-fetch-imgs-errors">
        <source>While fetching item images the following errors occured:</source>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalih-status-feeds-add-images-add-db">
        <source>Adding item image information to database</source>
        <extracomment>Status messages</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-command-places-summary">
        <source>Manage places</source>
        <extracomment>CLI command summary</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-places-add-name-desc">
        <source>Name of the place to add. (required)</source>
        <oldsource>Name of the place to add.</oldsource>
        <extracomment>CLI option description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-places-add-slug-desc">
        <source>Slug for this place used in URLs.</source>
        <extracomment>CLI option description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-places-add-parent-desc">
        <source>Database ID of a place this place belongs to. Like a district belonging to a city. Use %1 to get a list of places.</source>
        <translation type="unfinished"></translation>
    </message>
    <message id="statlihcmd-opt-value-dbid">
        <source>ID</source>
        <extracomment>CLI option value name</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-places-add-coords-desc">
        <source>Coordinates of the place. Enter them like &quot;%1;%2&quot;.</source>
        <extracomment>CLI option description, %1 will be replaced by the example latitude, %2 by the example longitude</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-places-add-coords-value">
        <source>latitude;longitude</source>
        <extracomment>CLI option value name</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-command-places-add-summary">
        <source>Add a new place</source>
        <extracomment>CLI command summary</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-command-places-add-description">
        <source>Adds a new place to the database.</source>
        <extracomment>CLI command description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-value-url">
        <source>URL</source>
        <extracomment>CLI option value name</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-places-add-link-desc">
        <source>URL of a website describing this place.</source>
        <extracomment>CLI option description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-places-add-description-desc">
        <source>Short description for this place.</source>
        <extracomment>CLI option description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-places-add-admin-id">
        <source>Administrative ID for this place.</source>
        <extracomment>CLI option description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-err-places-add-missing-name">
        <source>Please use the -n argument to set a name for this place.</source>
        <translation type="unfinished"></translation>
    </message>
    <message id="statlihcmd-err-places-add-invalid-parent">
        <source>Invalid parent ID. Has to be an integer value bigger than 0.</source>
        <oldsource>Invalid parent ID. Has to be an integer value bigger than 0.
</oldsource>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-err-feeds-add-invalid-link">
        <source>Invalid URL given for link option.</source>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-status-places-add-db-add">
        <source>Adding new place</source>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-err-places-add-nonhttp-link">
        <source>Link has to be a HTTP or HTTPS URL.</source>
        <translation type="unfinished"></translation>
    </message>
    <message id="statlihcmd-err-places-add-slug-in-use">
        <source>This slug is already in use by another place (%1, ID: %2).</source>
        <oldsource>This slug is already in use by another place (%1, ID: %2).
</oldsource>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-table-header-key">
        <source>Key</source>
        <extracomment>CLI table header</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-table-header-value">
        <source>Value</source>
        <extracomment>CLI table header</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-places-list-search-desc">
        <source>Search for the text in name or slug.</source>
        <extracomment>CLI option description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-status-query-db">
        <source>Querying database</source>
        <translation type="unfinished"></translation>
    </message>
    <message id="statlihcmd-warn-places-list-nothing-found">
        <source>Your query has not returned any result.</source>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-table-header-id">
        <source>ID</source>
        <extracomment>CLI table header</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-table-header-name">
        <source>Name</source>
        <extracomment>CLI table header</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-table-header-slug">
        <source>Slug</source>
        <extracomment>CLI table header</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-places-list-table-header-parentid">
        <source>Parent ID</source>
        <extracomment>CLI table header</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-places-list-table-header-adminid">
        <source>Administrative ID</source>
        <extracomment>CLI table header</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-places-list-table-header-coords">
        <source>Coordinates</source>
        <extracomment>CLI table header</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-table-header-link">
        <source>Link</source>
        <extracomment>CLI table header</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-table-header-created">
        <source>Created</source>
        <extracomment>CLI table header</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-table-header-updated">
        <source>Updated</source>
        <extracomment>CLI table header</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-command-places-list-summary">
        <source>List places</source>
        <extracomment>CLI command summary</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-command-places-list-description">
        <source>List places available in the database.</source>
        <extracomment>CLI command description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-opt-feeds-add-place-desc">
        <source>Database ID of the place this feed belongs to.</source>
        <extracomment>CLI option description</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-err-feeds-add-invalid-place-id">
        <source>Invalid place database ID.</source>
        <extracomment>Error message</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-status-feeds-add-checking-db">
        <source>Checking database</source>
        <translation type="unfinished"></translation>
    </message>
    <message id="statalihcmd-err-feeds-add-unknown-place">
        <source>Can not find a place with ID %1 in the database.</source>
        <extracomment>Error message</extracomment>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
