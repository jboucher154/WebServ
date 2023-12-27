// Assuming you have a function to detect when the user starts typing
function onCommentTyping() {
  document.querySelector('.comment_box').classList.remove('comment_text');
}

let repeatInterval;

// Add an event listener to the comment input for keydown
commentInput.addEventListener('keydown', function(event) {
  // Clear any existing interval
  clearInterval(repeatInterval);

  // Set up an interval to repeatedly add the pressed character to the comment box
  repeatInterval = setInterval(function() {

    // Check if the pressed key is Enter
    if (event.key !== 'Enter' && event.key !== 'Tab') {
      // Add the pressed character to the comment box
      commentInput.value += event.key;
    }
  }, 70); // Adjust the interval duration
  if (event.key === 'Enter') {
    // Clear any existing interval
    clearInterval(repeatInterval);

    // Call the addComment function immediately
    addComments(event);
  }
});

// Add an event listener to the comment input for keyup
commentInput.addEventListener('keyup', function() {
  // Clear the interval when the key is released
  clearInterval(repeatInterval);
});

function addComments(event) {
  var commentInput = document.querySelector('.comment_box');
  var commentContainer = document.querySelector('.comment_container');
  var commentText = commentInput.value.trim();

  if (commentText !== '') {
    // Create a new paragraph element for the comment
    var commentParagraph = document.createElement('p');

    // Append the comment to the comment container
    commentContainer.appendChild(commentParagraph);

    // Create a timestamp
    var timestamp = new Date();
    var timestampString = timestamp.toLocaleString();

    commentParagraph.innerHTML = `${timestampString}: <strong>${commentText}</strong>`;

    commentParagraph.style.fontSize = '0.8em'; // Adjust font size as needed

    // Clear the comment input
    commentInput.value = '';

    // Show the default placeholder in the comment input
    commentInput.placeholder = 'Write a comment here';

    // Display the comment container (if hidden)
    commentContainer.classList.remove('hidden');
    //add timestamp
  }
}