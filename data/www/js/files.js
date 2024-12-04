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
            <a class="filebtn upload" title="upload the selected file"><i class="fa-solid fa-upload" aria-hidden="true"></i></a>
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
        <a class="filebtn upload" title="upload the on the top selected file"><i class="fa-solid fa-upload" aria-hidden="true"></i></a>
        <a class="filebtn download" title="save the file on this computer"><i class="fa-solid fa-download" aria-hidden="true"></i></a>
        <a class="filebtn delete" title="delete the file"><i class="fa-solid fa-trash-arrow-up" aria-hidden="true"></i></a>
        <a class="filebtn show" title="show the file"><i class="fa-solid fa-magnifying-glass"></i></a>
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


function doShow() {
  // Check if the element with ID 'filePopup' exists
  if ($('#filePopup').length === 0) {
    // If it doesn't exist, create and append the popup modal and overlay
    var filePopupOverlay = $('<div id="filePopupOverlay"></div>');
    var filePopup = $('<div id="filePopup"><h3>File Content</h3><pre id="fileContent"></pre><button id="closePopup">Close</button></div>');

    // Append the popup and overlay to the body
    $('body').append(filePopupOverlay, filePopup);

    // Set initial styles for the popup
    $('#filePopup').css({
      'display': 'none',
      'position': 'fixed',
      'top': '50%',
      'left': '50%',
      'transform': 'translate(-50%, -50%)',
      'background': 'white',
      'padding': '20px',
      'box-shadow': '0px 0px 10px rgba(0, 0, 0, 0.1)',
      'z-index': '1000',
      'max-width': '90%',   // Optional: Control the max width of the popup
      'max-height': '80%'   // Optional: Limit the height to avoid covering the entire screen
    });

    // Set initial styles for the overlay
    $('#filePopupOverlay').css({
      'display': 'none',
      'position': 'fixed',
      'top': '0',
      'left': '0',
      'width': '100%',
      'height': '100%',
      'background': 'rgba(0, 0, 0, 0.5)',
      'z-index': '999'
    });

    // Set styles for file content with scrolling
    $('#fileContent').css({
      'max-height': '400px', // Set max height to enable scrolling
      'overflow-y': 'auto',  // Enable vertical scrolling
      'white-space': 'pre-wrap', // Maintain line breaks and wrap text
      'word-wrap': 'break-word' // Allow long words to break and wrap
    });

    // Close the popup when the "Close" button is clicked
    $('#closePopup').on('click', function () {
      $('#filePopup').fadeOut();
      $('#filePopupOverlay').fadeOut();
    });

    // Close the popup when clicking on the overlay
    $('#filePopupOverlay').on('click', function () {
      $('#filePopup').fadeOut();
      $('#filePopupOverlay').fadeOut();
    });
  }

  var filename = $(this).data('path'); // Get the filename from the data attribute
  if (!filename) return;  // If filename is not provided, exit

  downloadFileAsString(filename, function (data) {
    $('#fileContent').text(data);  // Display file content in the popup
    $('#filePopup').fadeIn();  // Show the popup
    $('#filePopupOverlay').fadeIn();  // Show the overlay
  })
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

            row.find('.show')
              .data("path", data[i].path)
              .off("click")
              .on('click', doShow);
            // Add the row to the table
            $("#filetable").append(row);
          }
        }
      } else {
        // Error message if response is not in array format
        $("#filetable").append('<tr><td colspan="4">Error: Invalid data structure received.</td></tr>');
        console.error("The loaded data is not an array.");
      }

      // Apply styling to table elements
      $(".colbtn").css({ "width": "9em" });
      $(".type").css({ "width": "7em" });
      $(".size").css({ "width": "7em", "text-align": "right" });
      $(".filebtn")
        .addClass('w3-button w3-hover-black w3-round-large')
        .css({ "padding-top": "0px", "padding-bottom": "0px", "padding-right": "4px", "padding-left": "4px" });
    },
    error: function (jqxhr, textStatus, error) {
      // Remove loading indicator and show error message
      $("#loading").remove();

      if (textStatus === "timeout") {
        // Timeout error message
        $("#fileTab").html(`<div id="loading">Error: Timeout while loading data.</div>`);
        console.error("Error: Timeout while loading data.");
      } else {
        // General error message for other issues (e.g., network errors)
        $("#fileTab").html(`<div id="loading">Error: ${textStatus} - ${error}</div>`);
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
  $("#fileInput")
    .off("change")
    .on('change', function () {
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
