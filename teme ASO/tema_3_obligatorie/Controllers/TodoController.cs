using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using TodoApp.Data;
using TodoApp.Models;

namespace TodoApp.Controllers;

public class TodoController : Controller
{
    private readonly AppDbContext _db;

    public TodoController(AppDbContext db)
    {
        _db = db;
    }

    public async Task<IActionResult> Index(string filter = "all")
    {
        var query = _db.TodoItems.AsQueryable();

        query = filter switch
        {
            "active"    => query.Where(t => !t.IsCompleted),
            "completed" => query.Where(t => t.IsCompleted),
            _           => query
        };

        var items = await query.OrderByDescending(t => t.CreatedAt).ToListAsync();
        ViewBag.Filter = filter;
        ViewBag.TotalCount     = await _db.TodoItems.CountAsync();
        ViewBag.CompletedCount = await _db.TodoItems.CountAsync(t => t.IsCompleted);
        return View(items);
    }

    public IActionResult Create() => View();

    [HttpPost]
    [ValidateAntiForgeryToken]
    public async Task<IActionResult> Create(TodoItem item)
    {
        if (!ModelState.IsValid)
            return View(item);

        item.CreatedAt = DateTime.UtcNow;
        _db.TodoItems.Add(item);
        await _db.SaveChangesAsync();
        return RedirectToAction(nameof(Index));
    }

    public async Task<IActionResult> Edit(int id)
    {
        var item = await _db.TodoItems.FindAsync(id);
        if (item is null) return NotFound();
        return View(item);
    }

    [HttpPost]
    [ValidateAntiForgeryToken]
    public async Task<IActionResult> Edit(int id, TodoItem item)
    {
        if (id != item.Id) return BadRequest();
        if (!ModelState.IsValid) return View(item);

        var existing = await _db.TodoItems.FindAsync(id);
        if (existing is null) return NotFound();

        existing.Title       = item.Title;
        existing.Description = item.Description;

        await _db.SaveChangesAsync();
        return RedirectToAction(nameof(Index));
    }

    [HttpPost]
    [ValidateAntiForgeryToken]
    public async Task<IActionResult> Toggle(int id)
    {
        var item = await _db.TodoItems.FindAsync(id);
        if (item is null) return NotFound();

        item.IsCompleted  = !item.IsCompleted;
        item.CompletedAt  = item.IsCompleted ? DateTime.UtcNow : null;
        await _db.SaveChangesAsync();
        return RedirectToAction(nameof(Index));
    }

    public async Task<IActionResult> Delete(int id)
    {
        var item = await _db.TodoItems.FindAsync(id);
        if (item is null) return NotFound();
        return View(item);
    }

    [HttpPost, ActionName("Delete")]
    [ValidateAntiForgeryToken]
    public async Task<IActionResult> DeleteConfirmed(int id)
    {
        var item = await _db.TodoItems.FindAsync(id);
        if (item is not null)
        {
            _db.TodoItems.Remove(item);
            await _db.SaveChangesAsync();
        }
        return RedirectToAction(nameof(Index));
    }
}
