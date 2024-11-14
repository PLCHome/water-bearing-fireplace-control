// URL to get the list of file names from the server
const GETFILENAMESURL = `/list?dir=/`;

// HTML template for the table header row, containing columns for buttons, file name, size, and type
const FILEHEAD = `
<tr>
    <th class="colbtn">&nbsp;</th>
    <th class="path">file</th>
    <th class="size">size</th>
    <th class="type">type</th>
</tr>
`;

// HTML template for the main file upload area, with input fields for uploading and naming files
const FILEAREA = `
<div id="myArea" style="height:100%; width:100%; overflow:auto" class="list-group" >
<table id="fileupload" class="w3-table-all w3-hoverable">
    <tr>
        <td class="colbtn">
            <a class="filebtn upload"><i class="fa-solid fa-upload" aria-hidden="true"></i></a>
        </td>
        <td>
            <input style="width:100%" type="file" id="fileInput"/> 
        </td>
        <td style="width:5em">
            <label for="filename">filename</label>
        </td>
        <td>
            <input style="width:100%" type="text" id="filename" />
        </td>
    </tr>
</table>
<div id="fileTab"/>
</div>`;

// HTML template for individual file rows in the file table, with buttons for upload, download, and delete actions
const FILEROW = `
<tr>
    <td class="colbtn">
        <a class="filebtn upload"><i class="fa-solid fa-upload" aria-hidden="true"></i></a>
        <a class="filebtn download"><i class="fa-solid fa-download" aria-hidden="true"></i></a>
        <a class="filebtn delete"><i class="fa-solid fa-trash-arrow-up" aria-hidden="true"></i></a>
    </td>
    <td class="path">&nbsp;</td>
    <td class="size">&nbsp;</td>
    <td class="type">&nbsp;</td>
</tr>
`;

// HTML template for the main table that will contain the list of files
const FILETAB = `
<table id="filetable" class="w3-table-all w3-hoverable"/>
`;

// Function to handle file upload
function doUpload() {
    // Retrieve the file from the file input field
    var fileInput = $("#fileInput");
    var file;
    if (fileInput && fileInput[0] && fileInput[0].files) {
        var file = fileInput[0].files[0]; // Select the first file if multiple are chosen
    }
    // Retrieve the filename from the data attribute or input field
    var filename = $(this).data('path');
    if (!filename || filename == "") {
        filename = $("#filename").val();
    }

    // Confirm with the user before uploading or overwriting
    if (file && filename && confirm(`Do you want to overwrite or upload the file ${filename} with ${file.name}?`)) {
        console.log("Calling uploadFile function...");
        // Call the upload function and refresh the file list after uploading
        uploadFile(file, filename, function () {
            buildFilelist()
        });
    }
}

// Function to handle file deletion
function doDelete() {
    // Get the URL of the file from the data attribute
    var fileUrl = $(this).data('path');
    // Check if the URL exists
    if (fileUrl && confirm(`Do you really want to delete the file ${fileUrl}?`)) {
        // Call deleteFile function and refresh the file list afterward
        deleteFile(fileUrl, function () {
            buildFilelist()
        });
    } else {
        // Error message if no URL is provided
        console.error("Error: No valid file URL.");
    }
}

// Function to handle file download
function doDownload() {
    // Get the URL of the file from the data attribute
    var fileUrl = $(this).data('path');
    // Check if the URL exists
    if (fileUrl) {
        // Call the downloadFile function
        downloadFile(fileUrl);
    } else {
        // Error message if no URL is provided
        console.error("Error: No valid file URL.");
    }
}

// Function to build and display the file list
function buildFilelist() {
    $("#fileTab").html('<div id="loading">Loading files...</div>');

    // Handle network errors, timeouts, and other issues
    $.ajax({
        url: GETFILENAMESURL,
        dataType: 'json',
        timeout: 5000, // Timeout set to 5 seconds
        success: function (data) {
            // Remove loading indicator once data is loaded
            $("#fileTab").html(FILETAB);
            $("#filetable").append(FILEHEAD);
    
            // Check if the response data is an array
            if (Array.isArray(data)) {
                if (data.length === 0) {
                    // No files found, display a message
                    $("#filetable").append('<tr><td colspan="4">No files found.</td></tr>');
                } else {
                    // For each file in data, add a row to the table
                    for (var i = 0; i < data.length; i++) {
                        let row = $(FILEROW);
                        keys = Object.keys(data[i]);
                        // Fill the row with file information
                        for (var k = 0; k < keys.length; k++) {
                            row.find(`.${keys[k]}`).text(data[i][keys[k]]);
                        }
                        row.data("row", i);
                        // Set up event handlers for each button in the row
                        row.find('.download')
                            .data("path", data[i].path)
                            .off("click")
                            .on('click', doDownload);

                        row.find('.delete')
                            .data("path", data[i].path)
                            .off("click")
                            .on('click', doDelete);

                        row.find('.upload')
                            .data("path", data[i].path)
                            .off("click")
                            .on('click', doUpload);
                        // Add the row to the table
                        $("#filetable").append(row);
                    }
                }

                // Apply styling to table elements
                $(".colbtn").css({ "width": "7em" });
                $(".type").css({ "width": "7em" });
                $(".size").css({ "width": "7em", "text-align": "right" });
                $(".filebtn")
                    .addClass('w3-button w3-hover-black w3-round-large')
                    .css({ "padding-top": "0px", "padding-bottom": "0px", "padding-right": "4px", "padding-left": "4px" });
            } else {
                // Error message if response is not in array format
                $("#filetable").append('<tr><td colspan="4">Error: Invalid data structure received.</td></tr>');
                console.error("The loaded data is not an array.");
            }
        },
        error: function (jqxhr, textStatus, error) {
            // Remove loading indicator and show error message
            $("#loading").remove();

            if (textStatus === "timeout") {
                // Timeout error message
                $("#filetable").append('<tr><td colspan="4">Error: Timeout while loading data.</td></tr>');
                console.error("Error: Timeout while loading data.");
            } else {
                // General error message for other issues (e.g., network errors)
                $("#filetable").append(`<tr><td colspan="4">Error: ${textStatus} - ${error}</td></tr>`);
                console.error(`Error loading JSON data: ${textStatus}, ${error}`);
            }
        }
    });
}

// Function to build the main file management page
function buildPageFiles() {
    // Insert file area HTML into #mySpace
    $("#mySpace").html(FILEAREA);
    // Update filename input field with the selected file’s name on file input change
    $("#fileInput").on('change', function () {
        $("#filename").val(this.files[0].name);
    });
    // Add styling to buttons and set up the upload button click handler
    $(".filebtn")
        .addClass('w3-button w3-hover-black w3-round-large')
        .css({ "padding-top": "0px", "padding-bottom": "0px", "padding-right": "4px", "padding-left": "4px" });
    $('.upload')
        .off("click")
        .on('click', doUpload);
    // Build the file list to display available files
    buildFilelist();
}
