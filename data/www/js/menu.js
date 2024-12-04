// Get the Sidebar
var mySidebar = document.getElementById("mySidebar");

// Get the DIV with overlay effect
var overlayBg = document.getElementById("myOverlay");

// Toggle between showing and hiding the sidebar, and add overlay effect
function w3_open() {
  if (overlayBg.style.display === 'block') {
    mySidebar.style.display = 'none';
    overlayBg.style.display = "none";
  } else {
    mySidebar.style.display = 'block';
    overlayBg.style.display = "block";
  }
}

// Close the sidebar with the close button
function w3_close() {
  mySidebar.style.display = "none";
  overlayBg.style.display = "none";
}

var showDim = document.getElementById('showDim');
var mySpace = document.getElementById('mySpace');
var myFooter = document.getElementById('myFooter');
var showDim = document.getElementById('showDim');
var myExtension = document.getElementById('myExtension');
var myExtensionToggle = document.getElementById('myExtensionToggle');

function setinner() {
  var height = window.innerHeight; // Höhe beim Laden anzeigen

  var setHeight = height - myFooter.offsetHeight - 44;
  if (setHeight < 100) {
    setHeight = 100;
  }
  mySpace.style.height = "" + setHeight + 'px';
}

function initMenu() {
  window.onresize = setinner;
  window.addEventListener('load', setinner);
  myExtension.style.display = 'none'
  setinner();
}

function myExtension_toggle() {
  if (myExtension.style.display === 'block') {
    myExtension.style.display = 'none';
    $("#myExtensionToggle").addClass("fa-arrow-up");
    $("#myExtensionToggle").removeClass("fa-arrow-down");
    //    myExtensionToggle.class="fa fa-caret-square-o-up";
  } else {
    myExtension.style.display = 'block';
    $("#myExtensionToggle").addClass("fa-arrow-down");
    $("#myExtensionToggle").removeClass("fa-arrow-up");
    //    myExtensionToggle.class="fa fa-caret-square-o-down";
  }
  setinner();
}
