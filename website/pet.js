function addComments() {
	
  }

// Assuming you have a function to detect when the user starts typing
function onCommentTyping() {
  document.querySelector('.comment_box').classList.remove('comment_text');
}

// Add an event listener to the textarea or input where the user writes a comment
document.getElementById('commentInput').addEventListener('input', onCommentTyping);